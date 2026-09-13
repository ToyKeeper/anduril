#!/usr/bin/env python3
"""
LED Colorimetry Tool for Anduril Flashlight Firmware
=====================================================

Calculates CIE XYZ tristimulus values for LEDs and generates
color mixing matrices for multi-channel flashlights.

Based on CIE 1931 2° Standard Observer color matching functions.

Usage:
    python3 led_colorimetry.py                      # Interactive demo
    python3 led_colorimetry.py --list               # List available LEDs
    python3 led_colorimetry.py --config d4k-rgbg    # Use preset config
    python3 led_colorimetry.py --leds R:630:20 G:528:35 B:465:25
    python3 led_colorimetry.py --leds R:630:20 G:528:35 G:528:35 B:465:25

See docs/color-science.md for theory and mathematical background.
"""

import argparse
import json
import sys
import numpy as np
from dataclasses import dataclass, field, asdict
from typing import Dict, List, Tuple, Optional
from pathlib import Path

# =============================================================================
# CIE 1931 2° Standard Observer Color Matching Functions
# =============================================================================

# Wavelength range: 380-780nm at 5nm intervals
# Data from CIE 15:2004
CIE_WAVELENGTHS = np.arange(380, 785, 5)

CIE_X_BAR = np.array([
    0.001368, 0.002236, 0.004243, 0.007650, 0.014310, 0.023190, 0.043510,
    0.077630, 0.134380, 0.214770, 0.283900, 0.328500, 0.348280, 0.348060,
    0.336200, 0.318700, 0.290800, 0.251100, 0.195360, 0.142100, 0.095640,
    0.058010, 0.032010, 0.014700, 0.004900, 0.002400, 0.009300, 0.029100,
    0.063270, 0.109600, 0.165500, 0.225750, 0.290400, 0.359700, 0.433450,
    0.512050, 0.594500, 0.678400, 0.762100, 0.842500, 0.916300, 0.978600,
    1.026300, 1.056700, 1.062200, 1.045600, 1.002600, 0.938400, 0.854450,
    0.751400, 0.642400, 0.541900, 0.447900, 0.360800, 0.283500, 0.218700,
    0.164900, 0.121200, 0.087400, 0.063600, 0.046770, 0.032900, 0.022700,
    0.015840, 0.011359, 0.008111, 0.005790, 0.004109, 0.002899, 0.002049,
    0.001440, 0.001000, 0.000690, 0.000476, 0.000332, 0.000235, 0.000166,
    0.000117, 0.000083, 0.000059, 0.000042
])

CIE_Y_BAR = np.array([
    0.000039, 0.000064, 0.000120, 0.000217, 0.000396, 0.000640, 0.001210,
    0.002180, 0.004000, 0.007300, 0.011600, 0.016840, 0.023000, 0.029800,
    0.038000, 0.048000, 0.060000, 0.073900, 0.090980, 0.112600, 0.139020,
    0.169300, 0.208020, 0.258600, 0.323000, 0.407300, 0.503000, 0.608200,
    0.710000, 0.793200, 0.862000, 0.914850, 0.954000, 0.980300, 0.994950,
    1.000000, 0.995000, 0.978600, 0.952000, 0.915400, 0.870000, 0.816300,
    0.757000, 0.694900, 0.631000, 0.566800, 0.503000, 0.441200, 0.381000,
    0.321000, 0.265000, 0.217000, 0.175000, 0.138200, 0.107000, 0.081600,
    0.061000, 0.044580, 0.032000, 0.023200, 0.017000, 0.011920, 0.008210,
    0.005723, 0.004102, 0.002929, 0.002091, 0.001484, 0.001047, 0.000740,
    0.000520, 0.000361, 0.000249, 0.000172, 0.000120, 0.000085, 0.000060,
    0.000042, 0.000030, 0.000021, 0.000015
])

CIE_Z_BAR = np.array([
    0.006450, 0.010550, 0.020050, 0.036210, 0.067850, 0.110200, 0.207400,
    0.371300, 0.645600, 1.039050, 1.385600, 1.622960, 1.747060, 1.782600,
    1.772110, 1.744100, 1.669200, 1.528100, 1.287640, 1.041900, 0.812950,
    0.616200, 0.465180, 0.353300, 0.272000, 0.212300, 0.158200, 0.111700,
    0.078250, 0.057250, 0.042160, 0.029840, 0.020300, 0.013400, 0.008750,
    0.005750, 0.003900, 0.002750, 0.002100, 0.001800, 0.001650, 0.001400,
    0.001100, 0.001000, 0.000800, 0.000600, 0.000340, 0.000240, 0.000190,
    0.000100, 0.000050, 0.000030, 0.000020, 0.000010, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000
])

# Standard illuminants
D65_XY = (0.31272, 0.32903)  # CIE Standard Illuminant D65 (6504K daylight)
D50_XY = (0.34567, 0.35850)  # CIE Standard Illuminant D50 (5003K horizon)
A_XY = (0.44757, 0.40745)    # CIE Standard Illuminant A (2856K incandescent)


# =============================================================================
# Planckian Locus (Blackbody Radiation)
# =============================================================================

def planckian_xy(T: float) -> Tuple[float, float]:
    """
    Calculate CIE xy chromaticity for a blackbody radiator at temperature T.

    Uses CIE approximation formulas (accurate to ~0.0001 for 1667K-25000K).

    Args:
        T: Color temperature in Kelvin

    Returns:
        Tuple of (x, y) chromaticity coordinates
    """
    # CIE 1960 UCS coordinates (more accurate approximation)
    if T < 1667:
        T = 1667  # Clamp to valid range
    if T > 25000:
        T = 25000

    # Approximation for x chromaticity (valid for 1667K to 25000K)
    if T <= 4000:
        x = (-0.2661239e9 / T**3 - 0.2343589e6 / T**2
             + 0.8776956e3 / T + 0.179910)
    else:
        x = (-3.0258469e9 / T**3 + 2.1070379e6 / T**2
             + 0.2226347e3 / T + 0.240390)

    # Approximation for y chromaticity
    if T <= 2222:
        y = (-1.1063814 * x**3 - 1.34811020 * x**2 + 2.18555832 * x - 0.20219683)
    elif T <= 4000:
        y = (-0.9549476 * x**3 - 1.37418593 * x**2 + 2.09137015 * x - 0.16748867)
    else:
        y = (3.0817580 * x**3 - 5.87338670 * x**2 + 3.75112997 * x - 0.37001483)

    return (x, y)


def generate_cct_lut(config: 'LightConfig',
                     temp_min: int = 2700,
                     temp_max: int = 6500,
                     steps: int = 32) -> List[Dict]:
    """
    Generate a lookup table of RGB values along the Planckian locus.

    For each CCT point, finds the optimal RGB mix to achieve that chromaticity
    within the gamut of the available LEDs.

    Args:
        config: Light configuration (needs R, G, B channels)
        temp_min: Minimum color temperature (K)
        temp_max: Maximum color temperature (K)
        steps: Number of steps in the LUT

    Returns:
        List of dicts with 'cct', 'x', 'y', 'r', 'g', 'b' values
    """
    # Get channel XYZ values
    channel_xyz = {}
    for ch in config.channels:
        channel_xyz[ch.name] = ch.effective_xyz()

    # We need at least R, G, B channels (or equivalent)
    # Try to identify them by chromaticity
    red_ch = None
    green_ch = None
    blue_ch = None

    for ch in config.channels:
        X, Y, Z = channel_xyz[ch.name]
        x, y = xyz_to_chromaticity(X, Y, Z)

        # Classify by chromaticity region
        if x > 0.5 and y < 0.4:  # Red region
            red_ch = ch
        elif y > 0.5:  # Green region
            green_ch = ch
        elif x < 0.2 and y < 0.2:  # Blue region
            blue_ch = ch

    if not (red_ch and green_ch and blue_ch):
        raise ValueError("Need R, G, B channels for CCT ramp generation")

    # Get XYZ for the three primaries
    Xr, Yr, Zr = channel_xyz[red_ch.name]
    Xg, Yg, Zg = channel_xyz[green_ch.name]
    Xb, Yb, Zb = channel_xyz[blue_ch.name]

    # Account for LED counts (e.g., 2 green LEDs)
    green_scale = 1.0 / green_ch.led_count  # Will need to scale down green

    lut = []
    temperatures = np.linspace(temp_min, temp_max, steps)

    for T in temperatures:
        target_x, target_y = planckian_xy(T)

        # Convert target xy to XYZ (assuming Y=1 for normalization)
        target_Y = 1.0
        target_X = (target_x / target_y) * target_Y
        target_Z = ((1 - target_x - target_y) / target_y) * target_Y

        # Solve for RGB using matrix inversion
        # [X]   [Xr Xg Xb] [r]
        # [Y] = [Yr Yg Yb] [g]
        # [Z]   [Zr Zg Zb] [b]
        M = np.array([
            [Xr, Xg * green_scale, Xb],
            [Yr, Yg * green_scale, Yb],
            [Zr, Zg * green_scale, Zb]
        ])

        try:
            M_inv = np.linalg.inv(M)
            rgb = M_inv @ np.array([target_X, target_Y, target_Z])

            # Normalize so max = 255, all values >= 0
            rgb = np.clip(rgb, 0, None)
            if rgb.max() > 0:
                rgb = rgb / rgb.max() * 255

            r, g, b = int(rgb[0]), int(rgb[1]), int(rgb[2])

            # The green value is already accounting for LED count
            # Apply additional scaling for RGBG
            g = int(g * green_scale)

            lut.append({
                'cct': int(T),
                'x': round(target_x, 4),
                'y': round(target_y, 4),
                'r': r,
                'g': g,
                'b': b
            })
        except np.linalg.LinAlgError:
            # Singular matrix, skip this point
            pass

    return lut


def generate_cct_c_code(lut: List[Dict], var_name: str = "cct_lut") -> str:
    """
    Generate C code for the CCT lookup table.

    Args:
        lut: Lookup table from generate_cct_lut()
        var_name: Variable name for the array

    Returns:
        C code string
    """
    lines = [
        "// CCT Ramp Lookup Table - RGB values along Planckian locus",
        "// Generated by tools/led_colorimetry.py",
        "//",
        "// Index 0 = warm (lowest CCT), Index N-1 = cool (highest CCT)",
        "// Values are {R, G_scaled, B} where G is pre-scaled for RGBG config",
        "",
        f"#define CCT_LUT_SIZE {len(lut)}",
        f"#define CCT_MIN {lut[0]['cct']}",
        f"#define CCT_MAX {lut[-1]['cct']}",
        "",
        "// CCT lookup table: {R, G, B}",
        f"PROGMEM const uint8_t {var_name}[CCT_LUT_SIZE][3] = {{",
    ]

    for i, entry in enumerate(lut):
        cct = entry['cct']
        r, g, b = entry['r'], entry['g'], entry['b']
        lines.append(f"    {{{r:3d}, {g:3d}, {b:3d}}},  // [{i:2d}] {cct}K")

    lines.append("};")
    lines.append("")

    return "\n".join(lines)


# =============================================================================
# Data Classes
# =============================================================================

@dataclass
class LED:
    """LED specification with spectral characteristics."""
    name: str
    dominant_wavelength: float  # nm
    fwhm: float = 25.0          # nm, Full Width at Half Maximum
    relative_flux: float = 1.0  # relative luminous flux (for multi-LED channels)

    def __post_init__(self):
        # Calculate Gaussian sigma from FWHM
        self.sigma = self.fwhm / (2 * np.sqrt(2 * np.log(2)))


@dataclass
class Channel:
    """A physical channel that may contain multiple LEDs."""
    name: str
    leds: List[LED]

    @property
    def led_count(self) -> int:
        return len(self.leds)

    def effective_xyz(self) -> Tuple[float, float, float]:
        """Sum XYZ contributions from all LEDs in this channel."""
        X_total, Y_total, Z_total = 0.0, 0.0, 0.0
        for led in self.leds:
            X, Y, Z = led_to_xyz(led)
            X_total += X * led.relative_flux
            Y_total += Y * led.relative_flux
            Z_total += Z * led.relative_flux
        return X_total, Y_total, Z_total


@dataclass
class LightConfig:
    """Configuration for a multi-channel flashlight."""
    name: str
    description: str
    channels: List[Channel]
    target_white: Tuple[float, float] = D65_XY  # Default target for white balance


# =============================================================================
# LED Database
# =============================================================================

LED_DATABASE: Dict[str, LED] = {
    # Cree XP-E2 colored LEDs
    "XPE2_ROYAL_BLUE": LED("XP-E2 Royal Blue", 450, 20),
    "XPE2_BLUE": LED("XP-E2 Blue", 465, 25),
    "XPE2_CYAN": LED("XP-E2 Cyan", 495, 30),
    "XPE2_GREEN": LED("XP-E2 Green", 528, 35),
    "XPE2_YELLOW": LED("XP-E2 Yellow", 585, 20),
    "XPE2_AMBER": LED("XP-E2 Amber", 590, 20),
    "XPE2_RED_ORANGE": LED("XP-E2 Red-Orange", 620, 20),
    "XPE2_RED": LED("XP-E2 Red", 630, 20),
    "XPE2_DEEP_RED": LED("XP-E2 Deep Red", 660, 20),

    # Luminus
    "SST20_DEEP_RED": LED("SST-20 Deep Red", 660, 20),

    # Osram
    "OSRAM_W1_BLUE": LED("Osram W1 Blue", 455, 20),
    "OSRAM_W2_BLUE": LED("Osram W2 Blue", 450, 20),
    "OSRAM_W2_GREEN": LED("Osram W2 Green", 530, 35),
    "OSRAM_W2_RED": LED("Osram W2 Red", 625, 20),

    # Generic (for custom configs)
    "GENERIC_RED": LED("Generic Red", 625, 20),
    "GENERIC_GREEN": LED("Generic Green", 525, 35),
    "GENERIC_BLUE": LED("Generic Blue", 460, 25),
    "GENERIC_CYAN": LED("Generic Cyan", 500, 30),
    "GENERIC_AMBER": LED("Generic Amber", 590, 20),
}


# =============================================================================
# Preset Configurations
# =============================================================================

PRESET_CONFIGS: Dict[str, LightConfig] = {
    # ==========================================================================
    # Emisar D4K 3-Channel Configurations
    # ==========================================================================
    "d4k-rgbg": LightConfig(
        name="Emisar D4K 3-Channel (RGBG)",
        description="D4K with Red, Green×2, Blue - most common config",
        channels=[
            Channel("MAIN2", [LED_DATABASE["XPE2_GREEN"], LED_DATABASE["XPE2_GREEN"]]),
            Channel("LED3", [LED_DATABASE["XPE2_RED"]]),
            Channel("LED4", [LED_DATABASE["XPE2_BLUE"]]),
        ]
    ),
    "d4k-rgbb": LightConfig(
        name="Emisar D4K 3-Channel (RGBB)",
        description="D4K with Red, Green, Blue×2 configuration",
        channels=[
            Channel("MAIN2", [LED_DATABASE["XPE2_BLUE"], LED_DATABASE["XPE2_BLUE"]]),
            Channel("LED3", [LED_DATABASE["XPE2_RED"]]),
            Channel("LED4", [LED_DATABASE["XPE2_GREEN"]]),
        ]
    ),
    "d4k-rggw": LightConfig(
        name="Emisar D4K 3-Channel (RGGW)",
        description="D4K with Red, Green×2, phosphor White",
        channels=[
            Channel("MAIN2", [LED_DATABASE["XPE2_GREEN"], LED_DATABASE["XPE2_GREEN"]]),
            Channel("LED3", [LED_DATABASE["XPE2_RED"]]),
            Channel("LED4", [LED("5000K White", 550, 100)]),  # Phosphor white
        ]
    ),

    # ==========================================================================
    # Emisar D4V2 2-Channel Configurations
    # ==========================================================================
    "d4v2-2ch": LightConfig(
        name="Emisar D4V2 2-Channel (Warm/Cool)",
        description="Standard D4V2 tint-ramping with warm and cool white",
        channels=[
            Channel("WARM", [LED("2700K Warm", 580, 100), LED("2700K Warm", 580, 100)]),
            Channel("COOL", [LED("6500K Cool", 550, 100), LED("6500K Cool", 550, 100)]),
        ]
    ),

    # ==========================================================================
    # Noctigon Configurations
    # ==========================================================================
    "kr4-2ch": LightConfig(
        name="Noctigon KR4 2-Channel",
        description="KR4 with warm/cool tint ramping",
        channels=[
            Channel("WARM", [LED("2700K Warm", 580, 100), LED("2700K Warm", 580, 100)]),
            Channel("COOL", [LED("5000K Neutral", 555, 100), LED("5000K Neutral", 555, 100)]),
        ]
    ),
    "dm11-rgb": LightConfig(
        name="Noctigon DM11 RGB Aux",
        description="DM11 with RGB auxiliary LEDs",
        channels=[
            Channel("RED", [LED_DATABASE["GENERIC_RED"]]),
            Channel("GREEN", [LED_DATABASE["GENERIC_GREEN"]]),
            Channel("BLUE", [LED_DATABASE["GENERIC_BLUE"]]),
        ]
    ),

    # ==========================================================================
    # Sofirn/BLF Configurations
    # ==========================================================================
    "lt1s-pro": LightConfig(
        name="Sofirn LT1S Pro (3-Channel)",
        description="Lantern with Red + Warm + Cool white channels",
        channels=[
            Channel("RED", [LED_DATABASE["XPE2_RED"]]),
            Channel("WARM", [LED("2700K Warm", 580, 100)]),
            Channel("COOL", [LED("5700K Cool", 555, 100)]),
        ]
    ),

    # ==========================================================================
    # Generic/Reference Configurations
    # ==========================================================================
    "rgb-standard": LightConfig(
        name="Standard RGB (equal count)",
        description="Reference: one LED per channel, equal balance",
        channels=[
            Channel("RED", [LED_DATABASE["GENERIC_RED"]]),
            Channel("GREEN", [LED_DATABASE["GENERIC_GREEN"]]),
            Channel("BLUE", [LED_DATABASE["GENERIC_BLUE"]]),
        ]
    ),
    "rgbw-standard": LightConfig(
        name="Standard RGBW (equal count)",
        description="Reference: RGB + phosphor white LED",
        channels=[
            Channel("RED", [LED_DATABASE["GENERIC_RED"]]),
            Channel("GREEN", [LED_DATABASE["GENERIC_GREEN"]]),
            Channel("BLUE", [LED_DATABASE["GENERIC_BLUE"]]),
            Channel("WHITE", [LED("5000K White", 550, 100)]),
        ]
    ),
    "2ch-warm-cool": LightConfig(
        name="2-Channel Warm/Cool",
        description="Generic 2-channel tint-ramping setup",
        channels=[
            Channel("WARM", [LED("2700K Warm White", 580, 100)]),
            Channel("COOL", [LED("6500K Cool White", 550, 100)]),
        ]
    ),

    # ==========================================================================
    # Specialty Color Configurations
    # ==========================================================================
    "rgbcya": LightConfig(
        name="RGBCYA (6-color)",
        description="Full spectrum: Red, Green, Blue, Cyan, Yellow, Amber",
        channels=[
            Channel("RED", [LED_DATABASE["XPE2_RED"]]),
            Channel("GREEN", [LED_DATABASE["XPE2_GREEN"]]),
            Channel("BLUE", [LED_DATABASE["XPE2_BLUE"]]),
            Channel("CYAN", [LED_DATABASE["XPE2_CYAN"]]),
            Channel("YELLOW", [LED_DATABASE["XPE2_YELLOW"]]),
            Channel("AMBER", [LED_DATABASE["XPE2_AMBER"]]),
        ]
    ),
    "photo-video": LightConfig(
        name="Photo/Video (High CRI)",
        description="R+A+W for high-CRI photo/video lighting",
        channels=[
            Channel("RED", [LED_DATABASE["SST20_DEEP_RED"]]),  # Deep red for CRI
            Channel("AMBER", [LED_DATABASE["XPE2_AMBER"]]),    # Fill gap
            Channel("WHITE", [LED("4000K Neutral", 560, 100)]),
        ]
    ),
}


# =============================================================================
# Core Colorimetry Functions
# =============================================================================

def gaussian_spd(wavelengths: np.ndarray, center: float, sigma: float) -> np.ndarray:
    """
    Generate a Gaussian spectral power distribution.

    Args:
        wavelengths: Array of wavelengths (nm)
        center: Center wavelength (dominant wavelength)
        sigma: Standard deviation (derived from FWHM)

    Returns:
        Normalized SPD array
    """
    return np.exp(-0.5 * ((wavelengths - center) / sigma) ** 2)


def spd_to_xyz(spd: np.ndarray, wavelengths: np.ndarray = CIE_WAVELENGTHS) -> Tuple[float, float, float]:
    """
    Convert spectral power distribution to CIE XYZ tristimulus values.

    Uses CIE 1931 2° Standard Observer color matching functions.

    Args:
        spd: Spectral power distribution (same length as wavelengths)
        wavelengths: Wavelength array (default: CIE standard 380-780nm)

    Returns:
        Tuple of (X, Y, Z) tristimulus values
    """
    # Interpolate CMFs if wavelengths don't match
    if len(wavelengths) != len(CIE_WAVELENGTHS) or not np.allclose(wavelengths, CIE_WAVELENGTHS):
        x_bar = np.interp(wavelengths, CIE_WAVELENGTHS, CIE_X_BAR)
        y_bar = np.interp(wavelengths, CIE_WAVELENGTHS, CIE_Y_BAR)
        z_bar = np.interp(wavelengths, CIE_WAVELENGTHS, CIE_Z_BAR)
    else:
        x_bar = CIE_X_BAR
        y_bar = CIE_Y_BAR
        z_bar = CIE_Z_BAR

    # Numerical integration (trapezoidal rule)
    X = np.trapezoid(spd * x_bar, wavelengths)
    Y = np.trapezoid(spd * y_bar, wavelengths)
    Z = np.trapezoid(spd * z_bar, wavelengths)

    return X, Y, Z


def led_to_xyz(led: LED) -> Tuple[float, float, float]:
    """
    Calculate CIE XYZ values for an LED using Gaussian SPD approximation.

    Args:
        led: LED specification

    Returns:
        Tuple of (X, Y, Z) tristimulus values
    """
    spd = gaussian_spd(CIE_WAVELENGTHS, led.dominant_wavelength, led.sigma)
    return spd_to_xyz(spd)


def xyz_to_chromaticity(X: float, Y: float, Z: float) -> Tuple[float, float]:
    """
    Convert XYZ to xy chromaticity coordinates.

    Args:
        X, Y, Z: Tristimulus values

    Returns:
        Tuple of (x, y) chromaticity coordinates
    """
    total = X + Y + Z
    if total < 1e-10:
        return (0.0, 0.0)
    return (X / total, Y / total)


def xyz_to_lab(X: float, Y: float, Z: float,
               ref_white: Tuple[float, float, float] = (95.047, 100.0, 108.883)) -> Tuple[float, float, float]:
    """
    Convert XYZ to CIELAB (L*a*b*) color space.

    Args:
        X, Y, Z: Tristimulus values
        ref_white: Reference white XYZ (default: D65)

    Returns:
        Tuple of (L*, a*, b*) values
    """
    Xn, Yn, Zn = ref_white

    def f(t):
        delta = 6/29
        if t > delta**3:
            return t ** (1/3)
        else:
            return t / (3 * delta**2) + 4/29

    L = 116 * f(Y/Yn) - 16
    a = 500 * (f(X/Xn) - f(Y/Yn))
    b = 200 * (f(Y/Yn) - f(Z/Zn))

    return L, a, b


# =============================================================================
# Channel Scaling Calculations
# =============================================================================

def calculate_white_balance(config: LightConfig, target_y: float = 0.3290) -> Dict[str, int]:
    """
    Calculate optimal channel scaling factors for white balance.

    For configurations with unequal LED counts per channel (like RGBG),
    calculates the scaling needed to achieve target white chromaticity.

    Args:
        config: Light configuration
        target_y: Target y-chromaticity (default: D65 = 0.3290)

    Returns:
        Dictionary mapping channel names to scale factors (0-255)
    """
    # Get XYZ for each channel
    channel_xyz = {}
    for ch in config.channels:
        channel_xyz[ch.name] = ch.effective_xyz()

    # For now, simple approach: find scale for channel with most LEDs
    max_leds = max(ch.led_count for ch in config.channels)

    scales = {}
    for ch in config.channels:
        if ch.led_count == max_leds:
            # This channel needs scaling down
            # Binary search for optimal scale
            X_ch, Y_ch, Z_ch = channel_xyz[ch.name]
            other_xyz = [(name, xyz) for name, xyz in channel_xyz.items() if name != ch.name]

            s_lo, s_hi = 0.01, 1.0
            for _ in range(50):
                s = (s_lo + s_hi) / 2

                # Calculate mixed chromaticity
                X_mix = s * X_ch + sum(xyz[0] for _, xyz in other_xyz)
                Y_mix = s * Y_ch + sum(xyz[1] for _, xyz in other_xyz)
                Z_mix = s * Z_ch + sum(xyz[2] for _, xyz in other_xyz)

                _, y_result = xyz_to_chromaticity(X_mix, Y_mix, Z_mix)

                if y_result > target_y:
                    s_hi = s
                else:
                    s_lo = s

            scales[ch.name] = int(256 * (s_lo + s_hi) / 2)
        else:
            scales[ch.name] = 255  # Full brightness

    return scales


def calculate_blend_scales(config: LightConfig,
                           active_channels: List[str],
                           target_xy: Optional[Tuple[float, float]] = None) -> Dict[str, int]:
    """
    Calculate scaling for specific channel blend modes.

    Args:
        config: Light configuration
        active_channels: List of channel names to include in blend
        target_xy: Optional target chromaticity (x, y)

    Returns:
        Dictionary mapping channel names to scale factors (0-255)
    """
    scales = {ch.name: 0 for ch in config.channels}

    # Get XYZ for active channels
    active_xyz = {}
    for ch in config.channels:
        if ch.name in active_channels:
            active_xyz[ch.name] = ch.effective_xyz()
            scales[ch.name] = 255

    if not target_xy:
        return scales

    # TODO: Optimization to hit target chromaticity
    # For now, return equal scaling for active channels
    return scales


# =============================================================================
# Output Generation
# =============================================================================

def generate_c_header(config: LightConfig, scales: Dict[str, int],
                      output_file: Optional[str] = None) -> str:
    """
    Generate C header file with color mixing coefficients for Anduril.

    Args:
        config: Light configuration
        scales: Channel scale factors
        output_file: Optional file path to write header

    Returns:
        Header file content as string
    """
    lines = [
        "// Auto-generated color mixing configuration for Anduril",
        f"// Configuration: {config.name}",
        f"// Description: {config.description}",
        "// Generated by tools/led_colorimetry.py",
        "//",
        "// See docs/color-science.md for theory and background.",
        "",
        "#pragma once",
        "",
        f"// Number of channels: {len(config.channels)}",
        "",
        "// Channel scaling factors (0-255, where 255 = 100%)",
        "// These values balance LED counts and chromaticity for neutral white",
    ]

    for ch in config.channels:
        X, Y, Z = ch.effective_xyz()
        x, y = xyz_to_chromaticity(X, Y, Z)
        scale = scales.get(ch.name, 255)
        led_info = ", ".join(f"{led.dominant_wavelength}nm" for led in ch.leds)
        lines.append(f"#define {ch.name}_SCALE {scale:3d}  // {ch.led_count}x LED ({led_info}), xy=({x:.3f}, {y:.3f})")

    lines.extend([
        "",
        "// Chromaticity data for reference",
        "/*",
    ])

    for ch in config.channels:
        X, Y, Z = ch.effective_xyz()
        x, y = xyz_to_chromaticity(X, Y, Z)
        L, a, b = xyz_to_lab(X, Y, Z)
        lines.append(f" * {ch.name}: {ch.led_count}x LED")
        for led in ch.leds:
            lines.append(f" *   - {led.name}: λ={led.dominant_wavelength}nm, FWHM={led.fwhm}nm")
        lines.append(f" *   XYZ=({X:.2f}, {Y:.2f}, {Z:.2f})")
        lines.append(f" *   xy=({x:.4f}, {y:.4f})")
        lines.append(f" *   L*a*b*=({L:.1f}, {a:.1f}, {b:.1f})")
        lines.append(" *")

    lines.append(" */")

    content = "\n".join(lines) + "\n"

    if output_file:
        Path(output_file).write_text(content)
        print(f"Wrote header to {output_file}")

    return content


def print_analysis(config: LightConfig, verbose: bool = True):
    """
    Print detailed colorimetric analysis of a light configuration.
    """
    print("=" * 70)
    print(f"Colorimetric Analysis: {config.name}")
    print(f"Description: {config.description}")
    print("=" * 70)

    print("\n--- Channel Details ---")
    total_X, total_Y, total_Z = 0, 0, 0

    for ch in config.channels:
        X, Y, Z = ch.effective_xyz()
        x, y = xyz_to_chromaticity(X, Y, Z)
        L, a, b = xyz_to_lab(X, Y, Z)

        total_X += X
        total_Y += Y
        total_Z += Z

        print(f"\n{ch.name} ({ch.led_count} LED{'s' if ch.led_count > 1 else ''}):")
        for led in ch.leds:
            print(f"  - {led.name}: λ={led.dominant_wavelength}nm, FWHM={led.fwhm}nm")
        print(f"  XYZ: ({X:.4f}, {Y:.4f}, {Z:.4f})")
        print(f"  xy chromaticity: ({x:.4f}, {y:.4f})")
        print(f"  L*a*b*: ({L:.1f}, {a:.1f}, {b:.1f})")

    # Equal PWM mix
    x_mix, y_mix = xyz_to_chromaticity(total_X, total_Y, total_Z)
    print(f"\n--- Equal-PWM Mix ---")
    print(f"Combined chromaticity: xy=({x_mix:.4f}, {y_mix:.4f})")
    print(f"D65 reference:         xy=({D65_XY[0]:.4f}, {D65_XY[1]:.4f})")

    # Calculate white balance
    print(f"\n--- White Balance Scaling ---")
    scales = calculate_white_balance(config)

    for ch_name, scale in scales.items():
        pct = scale / 256 * 100
        print(f"{ch_name}_SCALE = {scale:3d} ({pct:.1f}%)")

    # Calculate resulting chromaticity
    scaled_X, scaled_Y, scaled_Z = 0, 0, 0
    for ch in config.channels:
        X, Y, Z = ch.effective_xyz()
        s = scales.get(ch.name, 255) / 255
        scaled_X += X * s
        scaled_Y += Y * s
        scaled_Z += Z * s

    x_scaled, y_scaled = xyz_to_chromaticity(scaled_X, scaled_Y, scaled_Z)
    print(f"\nScaled chromaticity: xy=({x_scaled:.4f}, {y_scaled:.4f})")

    # Blend mode analysis
    if len(config.channels) >= 3:
        print(f"\n--- Blend Mode Scaling ---")

        # Get channel names
        ch_names = [ch.name for ch in config.channels]

        # Two-channel blends
        for i, ch1 in enumerate(config.channels):
            for ch2 in config.channels[i+1:]:
                print(f"\n{ch1.name} + {ch2.name}:")
                X1, Y1, Z1 = ch1.effective_xyz()
                X2, Y2, Z2 = ch2.effective_xyz()

                for ratio in [0.25, 0.5, 0.75]:
                    X_mix = X1 + ratio * X2
                    Y_mix = Y1 + ratio * Y2
                    Z_mix = Z1 + ratio * Z2
                    x_m, y_m = xyz_to_chromaticity(X_mix, Y_mix, Z_mix)
                    scale2 = int(256 * ratio)
                    print(f"  {ch2.name}_SCALE={scale2:3d}: xy=({x_m:.4f}, {y_m:.4f})")


# =============================================================================
# CLI Interface
# =============================================================================

def parse_led_spec(spec: str) -> LED:
    """
    Parse LED specification string.

    Format: NAME:WAVELENGTH:FWHM or NAME:WAVELENGTH
    Examples: R:630:20, G:528:35, B:465
    """
    parts = spec.split(':')
    if len(parts) < 2:
        raise ValueError(f"Invalid LED spec: {spec}. Format: NAME:WAVELENGTH[:FWHM]")

    name = parts[0]
    wavelength = float(parts[1])
    fwhm = float(parts[2]) if len(parts) > 2 else 25.0

    return LED(name, wavelength, fwhm)


def main():
    parser = argparse.ArgumentParser(
        description="LED Colorimetry Tool for Anduril Flashlight Firmware",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                           # Interactive demo with default RGBG config
  %(prog)s --list                    # List available LEDs and presets
  %(prog)s --config d4k-rgbg         # Use Emisar D4K RGBG preset
  %(prog)s --leds R:630:20 G:528:35 B:465:25  # Custom RGB config
  %(prog)s --leds R:630 G:528 G:528 B:465     # RGBG with auto FWHM
  %(prog)s --config d4k-rgbg --header hwdef_colors.h  # Generate C header

See docs/color-science.md for theory and mathematical background.
        """
    )

    parser.add_argument('--list', action='store_true',
                        help='List available LEDs and preset configurations')
    parser.add_argument('--config', '-c', choices=list(PRESET_CONFIGS.keys()),
                        help='Use a preset configuration')
    parser.add_argument('--leds', '-l', nargs='+', metavar='SPEC',
                        help='Custom LED specs: NAME:WAVELENGTH[:FWHM] ...')
    parser.add_argument('--header', '-H', metavar='FILE',
                        help='Generate C header file')
    parser.add_argument('--json', '-j', action='store_true',
                        help='Output results as JSON')
    parser.add_argument('--quiet', '-q', action='store_true',
                        help='Minimal output')
    parser.add_argument('--cct-lut', nargs=3, metavar=('MIN', 'MAX', 'STEPS'),
                        type=int,
                        help='Generate CCT lookup table: MIN_K MAX_K STEPS')

    args = parser.parse_args()

    # List mode
    if args.list:
        print("Available LEDs in database:")
        print("-" * 50)
        for key, led in sorted(LED_DATABASE.items()):
            print(f"  {key:20s}  λ={led.dominant_wavelength:3.0f}nm  FWHM={led.fwhm:2.0f}nm")

        print("\nPreset configurations:")
        print("-" * 50)
        for key, cfg in PRESET_CONFIGS.items():
            ch_summary = ", ".join(f"{ch.name}×{ch.led_count}" for ch in cfg.channels)
            print(f"  {key:15s}  {cfg.name}")
            print(f"                   Channels: {ch_summary}")
        return

    # Determine configuration
    if args.config:
        config = PRESET_CONFIGS[args.config]
    elif args.leds:
        # Build config from LED specs
        leds = [parse_led_spec(spec) for spec in args.leds]

        # Group by name to form channels
        channels = []
        ch_leds = {}
        for led in leds:
            if led.name not in ch_leds:
                ch_leds[led.name] = []
            ch_leds[led.name].append(led)

        for name, led_list in ch_leds.items():
            channels.append(Channel(name, led_list))

        config = LightConfig(
            name="Custom Configuration",
            description=f"Custom: {', '.join(args.leds)}",
            channels=channels
        )
    else:
        # Default demo
        config = PRESET_CONFIGS["d4k-rgbg"]

    # Calculate scales
    scales = calculate_white_balance(config)

    # CCT LUT generation mode
    if args.cct_lut:
        temp_min, temp_max, steps = args.cct_lut
        try:
            lut = generate_cct_lut(config, temp_min, temp_max, steps)
            print(generate_cct_c_code(lut))
        except ValueError as e:
            print(f"Error: {e}", file=sys.stderr)
            sys.exit(1)
        return

    # Output
    if args.json:
        result = {
            "config": {
                "name": config.name,
                "description": config.description,
                "channels": [
                    {
                        "name": ch.name,
                        "led_count": ch.led_count,
                        "leds": [asdict(led) for led in ch.leds],
                        "xyz": ch.effective_xyz(),
                        "chromaticity": xyz_to_chromaticity(*ch.effective_xyz())
                    }
                    for ch in config.channels
                ]
            },
            "scales": scales
        }
        print(json.dumps(result, indent=2))
    elif args.header:
        header = generate_c_header(config, scales, args.header)
        if not args.quiet:
            print(header)
    else:
        print_analysis(config, verbose=not args.quiet)

        if not args.quiet:
            print("\n--- Generated C Header ---")
            print(generate_c_header(config, scales))


if __name__ == "__main__":
    main()
