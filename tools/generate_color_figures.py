#!/usr/bin/env python3
"""
Generate color science figures for Anduril documentation.

Creates publication-quality charts with accurate spectral colors:
- CIE 1931 Color Matching Functions with spectrum background
- xy Chromaticity diagram with spectral locus
- LED SPD comparison chart
- RGBG mixing demonstration

Output: PNG files for docs/images/
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon, FancyBboxPatch, Circle
from matplotlib.collections import LineCollection
from matplotlib.colors import LinearSegmentedColormap
from pathlib import Path

# Import CMF data from our colorimetry tool
from led_colorimetry import (
    CIE_WAVELENGTHS, CIE_X_BAR, CIE_Y_BAR, CIE_Z_BAR,
    LED_DATABASE, gaussian_spd, led_to_xyz, xyz_to_chromaticity
)

# Output directory
OUTPUT_DIR = Path(__file__).parent.parent / "docs" / "images"


def wavelength_to_rgb(wavelength: float, gamma: float = 0.8) -> tuple:
    """
    Convert wavelength (nm) to RGB color.

    Based on CIE 1931 color matching functions with gamma correction.
    Returns colors suitable for display (sRGB gamut approximation).
    """
    if wavelength < 380 or wavelength > 780:
        return (0.0, 0.0, 0.0)

    # Peicewise linear approximation for visible spectrum
    if wavelength < 440:
        r = -(wavelength - 440) / (440 - 380)
        g = 0.0
        b = 1.0
    elif wavelength < 490:
        r = 0.0
        g = (wavelength - 440) / (490 - 440)
        b = 1.0
    elif wavelength < 510:
        r = 0.0
        g = 1.0
        b = -(wavelength - 510) / (510 - 490)
    elif wavelength < 580:
        r = (wavelength - 510) / (580 - 510)
        g = 1.0
        b = 0.0
    elif wavelength < 645:
        r = 1.0
        g = -(wavelength - 645) / (645 - 580)
        b = 0.0
    else:
        r = 1.0
        g = 0.0
        b = 0.0

    # Intensity falloff at edges of visible spectrum
    if wavelength < 420:
        factor = 0.3 + 0.7 * (wavelength - 380) / (420 - 380)
    elif wavelength > 700:
        factor = 0.3 + 0.7 * (780 - wavelength) / (780 - 700)
    else:
        factor = 1.0

    # Apply gamma correction
    r = (r * factor) ** gamma
    g = (g * factor) ** gamma
    b = (b * factor) ** gamma

    return (r, g, b)


def create_spectrum_colormap():
    """Create a colormap representing the visible spectrum."""
    wavelengths = np.linspace(380, 780, 256)
    colors = [wavelength_to_rgb(w) for w in wavelengths]
    return LinearSegmentedColormap.from_list('spectrum', colors)


def add_spectrum_background(ax, ymin, ymax, alpha=0.3):
    """Add a semi-transparent spectrum gradient to the background."""
    wavelengths = np.linspace(380, 780, 400)

    for i in range(len(wavelengths) - 1):
        w = wavelengths[i]
        color = wavelength_to_rgb(w)
        ax.axvspan(w, wavelengths[i+1], color=color, alpha=alpha, linewidth=0)


def plot_cmf_with_spectrum():
    """
    Plot CIE 1931 Color Matching Functions with spectrum background.
    """
    fig, ax = plt.subplots(figsize=(12, 6))

    # Add spectrum background
    add_spectrum_background(ax, 0, 1.8, alpha=0.15)

    # Plot CMFs
    ax.plot(CIE_WAVELENGTHS, CIE_X_BAR, 'r-', linewidth=2.5, label=r'$\bar{x}(\lambda)$')
    ax.plot(CIE_WAVELENGTHS, CIE_Y_BAR, 'g-', linewidth=2.5, label=r'$\bar{y}(\lambda)$')
    ax.plot(CIE_WAVELENGTHS, CIE_Z_BAR, 'b-', linewidth=2.5, label=r'$\bar{z}(\lambda)$')

    # Styling
    ax.set_xlim(380, 780)
    ax.set_ylim(0, 1.9)
    ax.set_xlabel('Wavelength (nm)', fontsize=12)
    ax.set_ylabel('Tristimulus Value', fontsize=12)
    ax.set_title('CIE 1931 2° Standard Observer Color Matching Functions', fontsize=14)
    ax.legend(loc='upper right', fontsize=12)
    ax.grid(True, alpha=0.3)

    # Add wavelength labels
    color_labels = [
        (400, 'Violet'), (450, 'Blue'), (490, 'Cyan'),
        (530, 'Green'), (570, 'Yellow'), (600, 'Orange'), (650, 'Red')
    ]
    for wl, name in color_labels:
        ax.annotate(name, xy=(wl, -0.12), ha='center', fontsize=9,
                   color=wavelength_to_rgb(wl))

    plt.tight_layout()
    return fig


def calculate_spectral_locus():
    """Calculate xy chromaticity coordinates for monochromatic wavelengths."""
    x_coords = []
    y_coords = []

    for i, wl in enumerate(CIE_WAVELENGTHS):
        X = CIE_X_BAR[i]
        Y = CIE_Y_BAR[i]
        Z = CIE_Z_BAR[i]
        total = X + Y + Z
        if total > 0.001:
            x_coords.append(X / total)
            y_coords.append(Y / total)

    return np.array(x_coords), np.array(y_coords)


def plot_chromaticity_diagram():
    """
    Plot CIE 1931 xy chromaticity diagram with spectral locus.
    """
    fig, ax = plt.subplots(figsize=(10, 10))

    # Calculate spectral locus
    x_locus, y_locus = calculate_spectral_locus()

    # Create filled gamut area with gradient
    # First, fill with a neutral color
    locus_points = list(zip(x_locus, y_locus))
    locus_points.append((x_locus[-1], y_locus[-1]))  # Close the curve
    # Add purple line (connect red to blue)
    locus_points.append((x_locus[0], y_locus[0]))

    polygon = Polygon(locus_points, closed=True, facecolor='#f0f0f0',
                     edgecolor='none', alpha=0.5)
    ax.add_patch(polygon)

    # Draw spectral locus with colors
    for i in range(len(CIE_WAVELENGTHS) - 1):
        if i < len(x_locus) - 1:
            wl = CIE_WAVELENGTHS[i]
            color = wavelength_to_rgb(wl)
            ax.plot([x_locus[i], x_locus[i+1]], [y_locus[i], y_locus[i+1]],
                   color=color, linewidth=4)

    # Purple line (non-spectral)
    ax.plot([x_locus[-1], x_locus[0]], [y_locus[-1], y_locus[0]],
           color='purple', linewidth=2, linestyle='--', alpha=0.7)

    # Add wavelength labels along the locus
    label_wavelengths = [380, 460, 480, 500, 520, 540, 560, 580, 600, 620, 700]
    for wl in label_wavelengths:
        idx = np.argmin(np.abs(CIE_WAVELENGTHS[:len(x_locus)] - wl))
        x, y = x_locus[idx], y_locus[idx]
        # Offset label outward
        cx, cy = 0.33, 0.33  # Center of diagram
        dx, dy = x - cx, y - cy
        dist = np.sqrt(dx**2 + dy**2)
        offset = 0.04
        lx = x + offset * dx / dist
        ly = y + offset * dy / dist
        ax.annotate(f'{wl}', xy=(x, y), xytext=(lx, ly),
                   fontsize=8, ha='center', color=wavelength_to_rgb(wl))

    # Plot standard illuminants
    illuminants = {
        'D65': (0.3127, 0.3290, '6500K Daylight'),
        'D50': (0.3457, 0.3585, '5000K Horizon'),
        'A': (0.4476, 0.4074, '2856K Tungsten'),
    }
    for name, (x, y, desc) in illuminants.items():
        ax.plot(x, y, 'k*', markersize=12)
        ax.annotate(f'{name}\n({desc})', xy=(x, y), xytext=(x+0.02, y-0.04),
                   fontsize=9, ha='left')

    # Plot RGB LED gamut triangle
    leds = ['XPE2_RED', 'XPE2_GREEN', 'XPE2_BLUE']
    led_xy = []
    colors = ['#CC0000', '#00CC00', '#0066FF']

    for led_name, color in zip(leds, colors):
        led = LED_DATABASE[led_name]
        X, Y, Z = led_to_xyz(led)
        x, y = xyz_to_chromaticity(X, Y, Z)
        led_xy.append((x, y))
        ax.plot(x, y, 'o', color=color, markersize=12, markeredgecolor='black')
        ax.annotate(f'{led.dominant_wavelength}nm', xy=(x, y),
                   xytext=(x+0.02, y+0.02), fontsize=10, color=color)

    # Draw gamut triangle
    triangle = Polygon(led_xy, closed=True, fill=False,
                      edgecolor='black', linewidth=2, linestyle='-')
    ax.add_patch(triangle)
    ax.annotate('RGB LED\nGamut', xy=(0.35, 0.4), fontsize=10, ha='center')

    # Styling
    ax.set_xlim(0, 0.8)
    ax.set_ylim(0, 0.9)
    ax.set_xlabel('x', fontsize=14)
    ax.set_ylabel('y', fontsize=14)
    ax.set_title('CIE 1931 xy Chromaticity Diagram', fontsize=14)
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    return fig


def plot_led_spd_comparison():
    """
    Plot SPD comparison of different LEDs.
    """
    fig, ax = plt.subplots(figsize=(12, 6))

    # Add spectrum background
    add_spectrum_background(ax, 0, 1.1, alpha=0.1)

    wavelengths = np.linspace(380, 780, 500)

    # LEDs to plot
    leds_to_plot = [
        ('XPE2_RED', '#CC0000', 'Red (630nm)'),
        ('XPE2_GREEN', '#00AA00', 'Green (528nm)'),
        ('XPE2_BLUE', '#0066FF', 'Blue (465nm)'),
        ('XPE2_CYAN', '#00CCCC', 'Cyan (495nm)'),
        ('XPE2_AMBER', '#FF8800', 'Amber (590nm)'),
    ]

    for led_name, color, label in leds_to_plot:
        led = LED_DATABASE[led_name]
        spd = gaussian_spd(wavelengths, led.dominant_wavelength, led.sigma)
        ax.fill_between(wavelengths, spd, alpha=0.3, color=color)
        ax.plot(wavelengths, spd, color=color, linewidth=2, label=label)

    # Styling
    ax.set_xlim(380, 780)
    ax.set_ylim(0, 1.1)
    ax.set_xlabel('Wavelength (nm)', fontsize=12)
    ax.set_ylabel('Relative Intensity', fontsize=12)
    ax.set_title('LED Spectral Power Distributions (Gaussian Approximation)', fontsize=14)
    ax.legend(loc='upper right', fontsize=10)
    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    return fig


def plot_rgbg_mixing():
    """
    Demonstrate the RGBG color mixing problem and solution.
    """
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    # Calculate chromaticity points
    red = LED_DATABASE['XPE2_RED']
    green = LED_DATABASE['XPE2_GREEN']
    blue = LED_DATABASE['XPE2_BLUE']

    r_xyz = led_to_xyz(red)
    g_xyz = led_to_xyz(green)
    b_xyz = led_to_xyz(blue)

    r_xy = xyz_to_chromaticity(*r_xyz)
    g_xy = xyz_to_chromaticity(*g_xyz)
    b_xy = xyz_to_chromaticity(*b_xyz)

    d65_xy = (0.3127, 0.3290)

    def draw_base_diagram(ax, title):
        """Draw base chromaticity diagram on axis."""
        x_locus, y_locus = calculate_spectral_locus()

        # Spectral locus
        for i in range(len(CIE_WAVELENGTHS) - 1):
            if i < len(x_locus) - 1:
                wl = CIE_WAVELENGTHS[i]
                color = wavelength_to_rgb(wl)
                ax.plot([x_locus[i], x_locus[i+1]], [y_locus[i], y_locus[i+1]],
                       color=color, linewidth=2, alpha=0.5)

        # LED positions
        ax.plot(*r_xy, 'o', color='#CC0000', markersize=15, markeredgecolor='black', label='Red')
        ax.plot(*g_xy, 'o', color='#00CC00', markersize=15, markeredgecolor='black', label='Green')
        ax.plot(*b_xy, 'o', color='#0066FF', markersize=15, markeredgecolor='black', label='Blue')
        ax.plot(*d65_xy, '*', color='black', markersize=15, label='D65 Target')

        ax.set_xlim(0.1, 0.75)
        ax.set_ylim(0, 0.85)
        ax.set_xlabel('x', fontsize=11)
        ax.set_ylabel('y', fontsize=11)
        ax.set_title(title, fontsize=12)
        ax.set_aspect('equal')
        ax.grid(True, alpha=0.3)

    # Panel 1: RGB equal mix
    ax1 = axes[0]
    draw_base_diagram(ax1, 'RGB Equal Mix (1:1:1)')

    # Calculate equal mix
    X_eq = r_xyz[0] + g_xyz[0] + b_xyz[0]
    Y_eq = r_xyz[1] + g_xyz[1] + b_xyz[1]
    Z_eq = r_xyz[2] + g_xyz[2] + b_xyz[2]
    eq_xy = xyz_to_chromaticity(X_eq, Y_eq, Z_eq)

    ax1.plot(*eq_xy, 's', color='white', markersize=12, markeredgecolor='black',
            markeredgewidth=2, label=f'Result\nxy=({eq_xy[0]:.3f}, {eq_xy[1]:.3f})')
    ax1.annotate('', xy=eq_xy, xytext=d65_xy,
                arrowprops=dict(arrowstyle='->', color='gray', lw=2))
    ax1.legend(loc='upper right', fontsize=8)

    # Panel 2: RGBG equal mix (too green)
    ax2 = axes[1]
    draw_base_diagram(ax2, 'RGBG Equal Mix (1:2:1) - Too Green!')

    # Calculate RGBG equal mix (2x green)
    X_rgbg = r_xyz[0] + 2*g_xyz[0] + b_xyz[0]
    Y_rgbg = r_xyz[1] + 2*g_xyz[1] + b_xyz[1]
    Z_rgbg = r_xyz[2] + 2*g_xyz[2] + b_xyz[2]
    rgbg_xy = xyz_to_chromaticity(X_rgbg, Y_rgbg, Z_rgbg)

    ax2.plot(*rgbg_xy, 's', color='#88FF88', markersize=12, markeredgecolor='darkgreen',
            markeredgewidth=2, label=f'Result\nxy=({rgbg_xy[0]:.3f}, {rgbg_xy[1]:.3f})')
    ax2.annotate('Too green!', xy=rgbg_xy, xytext=(rgbg_xy[0]-0.1, rgbg_xy[1]+0.05),
                fontsize=10, color='darkgreen',
                arrowprops=dict(arrowstyle='->', color='darkgreen'))
    ax2.legend(loc='upper right', fontsize=8)

    # Panel 3: RGBG with correction
    ax3 = axes[2]
    draw_base_diagram(ax3, 'RGBG Corrected (scale=105/256)')

    # Calculate corrected mix
    scale = 105/256
    X_corr = r_xyz[0] + 2*scale*g_xyz[0] + b_xyz[0]
    Y_corr = r_xyz[1] + 2*scale*g_xyz[1] + b_xyz[1]
    Z_corr = r_xyz[2] + 2*scale*g_xyz[2] + b_xyz[2]
    corr_xy = xyz_to_chromaticity(X_corr, Y_corr, Z_corr)

    ax3.plot(*corr_xy, 's', color='white', markersize=12, markeredgecolor='black',
            markeredgewidth=2, label=f'Result\nxy=({corr_xy[0]:.3f}, {corr_xy[1]:.3f})')
    ax3.annotate('D65 White!', xy=corr_xy, xytext=(corr_xy[0]+0.05, corr_xy[1]+0.08),
                fontsize=10, color='black',
                arrowprops=dict(arrowstyle='->', color='black'))
    ax3.legend(loc='upper right', fontsize=8)

    plt.tight_layout()
    return fig


def plot_spectrum_bar():
    """
    Create a simple horizontal spectrum bar for reference.
    """
    fig, ax = plt.subplots(figsize=(12, 1.5))

    wavelengths = np.linspace(380, 780, 400)

    for i in range(len(wavelengths) - 1):
        w = wavelengths[i]
        color = wavelength_to_rgb(w)
        ax.axvspan(w, wavelengths[i+1], color=color, alpha=1.0)

    ax.set_xlim(380, 780)
    ax.set_ylim(0, 1)
    ax.set_xlabel('Wavelength (nm)', fontsize=12)
    ax.set_yticks([])
    ax.set_title('Visible Light Spectrum', fontsize=12)

    # Add wavelength markers
    for wl in [400, 450, 500, 550, 600, 650, 700, 750]:
        ax.axvline(wl, color='white', linewidth=0.5, alpha=0.5)

    plt.tight_layout()
    return fig


def plot_color_modes():
    """
    Show all color modes with their chromaticity positions and derivation.
    """
    fig, ax = plt.subplots(figsize=(12, 10))

    # Calculate spectral locus
    x_locus, y_locus = calculate_spectral_locus()

    # Draw spectral locus with colors
    for i in range(len(CIE_WAVELENGTHS) - 1):
        if i < len(x_locus) - 1:
            wl = CIE_WAVELENGTHS[i]
            color = wavelength_to_rgb(wl)
            ax.plot([x_locus[i], x_locus[i+1]], [y_locus[i], y_locus[i+1]],
                   color=color, linewidth=3)

    # Purple line
    ax.plot([x_locus[-1], x_locus[0]], [y_locus[-1], y_locus[0]],
           color='purple', linewidth=2, linestyle='--', alpha=0.7)

    # Get LED chromaticities
    red = LED_DATABASE['XPE2_RED']
    green = LED_DATABASE['XPE2_GREEN']
    blue = LED_DATABASE['XPE2_BLUE']

    r_xyz = led_to_xyz(red)
    g_xyz = led_to_xyz(green)
    b_xyz = led_to_xyz(blue)

    r_xy = xyz_to_chromaticity(*r_xyz)
    g_xy = xyz_to_chromaticity(*g_xyz)
    b_xy = xyz_to_chromaticity(*b_xyz)

    # Draw gamut triangle
    triangle = Polygon([r_xy, g_xy, b_xy], closed=True, fill=True,
                      facecolor='#f0f0f0', edgecolor='black', linewidth=2, alpha=0.3)
    ax.add_patch(triangle)

    # Plot LED positions
    ax.plot(*r_xy, 'o', color='#CC0000', markersize=20, markeredgecolor='black', markeredgewidth=2)
    ax.plot(*g_xy, 'o', color='#00CC00', markersize=20, markeredgecolor='black', markeredgewidth=2)
    ax.plot(*b_xy, 'o', color='#0066FF', markersize=20, markeredgecolor='black', markeredgewidth=2)

    ax.annotate('Red LED\n630nm', xy=r_xy, xytext=(r_xy[0]+0.02, r_xy[1]-0.06),
               fontsize=10, ha='left')
    ax.annotate('Green LED\n528nm', xy=g_xy, xytext=(g_xy[0]-0.08, g_xy[1]+0.02),
               fontsize=10, ha='center')
    ax.annotate('Blue LED\n465nm', xy=b_xy, xytext=(b_xy[0]-0.02, b_xy[1]-0.05),
               fontsize=10, ha='right')

    # Define color modes with their calculations
    modes = []

    # Mode 1: D65 White (RGBG with scale=105)
    scale_white = 105/256
    X_w = r_xyz[0] + 2*scale_white*g_xyz[0] + b_xyz[0]
    Y_w = r_xyz[1] + 2*scale_white*g_xyz[1] + b_xyz[1]
    Z_w = r_xyz[2] + 2*scale_white*g_xyz[2] + b_xyz[2]
    white_xy = xyz_to_chromaticity(X_w, Y_w, Z_w)
    modes.append(('D65 White', white_xy, '#FFFFFF', 'black',
                  'R:255 + G:105×2 + B:255\nMAIN2_SCALE=105'))

    # Mode 2: Yellow/Orange (R + G,G with scale=32)
    scale_yellow = 32/256
    X_y = r_xyz[0] + 2*scale_yellow*g_xyz[0]
    Y_y = r_xyz[1] + 2*scale_yellow*g_xyz[1]
    Z_y = r_xyz[2] + 2*scale_yellow*g_xyz[2]
    yellow_xy = xyz_to_chromaticity(X_y, Y_y, Z_y)
    modes.append(('Yellow/Orange', yellow_xy, '#FFAA00', 'black',
                  'R:255 + G:32×2 + B:0\nMAIN2_SCALE_RGG=32'))

    # Mode 3: Cyan (G,G + B with scale=128)
    scale_cyan = 128/256
    X_c = 2*scale_cyan*g_xyz[0] + b_xyz[0]
    Y_c = 2*scale_cyan*g_xyz[1] + b_xyz[1]
    Z_c = 2*scale_cyan*g_xyz[2] + b_xyz[2]
    cyan_xy = xyz_to_chromaticity(X_c, Y_c, Z_c)
    modes.append(('Cyan', cyan_xy, '#00CCCC', 'black',
                  'R:0 + G:128×2 + B:255\nMAIN2_SCALE_GGB=128'))

    # Mode 4: Purple (R + B only)
    X_p = r_xyz[0] + b_xyz[0]
    Y_p = r_xyz[1] + b_xyz[1]
    Z_p = r_xyz[2] + b_xyz[2]
    purple_xy = xyz_to_chromaticity(X_p, Y_p, Z_p)
    modes.append(('Purple', purple_xy, '#CC00CC', 'white',
                  'R:255 + G:0 + B:255\n(No green)'))

    # Mode 5: Pure Green (G,G only)
    modes.append(('Green', g_xy, '#00CC00', 'black',
                  'R:0 + G:255×2 + B:0\n(Green LEDs only)'))

    # Mode 6: Pure Red
    modes.append(('Red', r_xy, '#CC0000', 'white',
                  'R:255 + G:0 + B:0\n(Red LED only)'))

    # Mode 7: Pure Blue
    modes.append(('Blue', b_xy, '#0066FF', 'white',
                  'R:0 + G:0 + B:255\n(Blue LED only)'))

    # Plot all modes
    for name, xy, facecolor, textcolor, formula in modes:
        if name not in ['Red', 'Green', 'Blue']:  # Already plotted LEDs
            ax.plot(*xy, 's', color=facecolor, markersize=18,
                   markeredgecolor='black', markeredgewidth=2)

        # Position annotations to avoid overlap
        if name == 'D65 White':
            offset = (0.04, 0.02)
        elif name == 'Yellow/Orange':
            offset = (0.03, 0.03)
        elif name == 'Cyan':
            offset = (-0.15, 0.02)
        elif name == 'Purple':
            offset = (0.03, -0.02)
        else:
            continue  # Skip LED labels (already done)

        ax.annotate(f'{name}\nxy=({xy[0]:.3f}, {xy[1]:.3f})\n{formula}',
                   xy=xy, xytext=(xy[0]+offset[0], xy[1]+offset[1]),
                   fontsize=9, ha='left',
                   bbox=dict(boxstyle='round,pad=0.3', facecolor=facecolor,
                            edgecolor='black', alpha=0.8),
                   color=textcolor)

    # Draw lines showing mixing paths
    # Yellow: R to G line
    ax.plot([r_xy[0], g_xy[0]], [r_xy[1], g_xy[1]], 'k--', alpha=0.3, linewidth=1)
    # Cyan: G to B line
    ax.plot([g_xy[0], b_xy[0]], [g_xy[1], b_xy[1]], 'k--', alpha=0.3, linewidth=1)
    # Purple: R to B line
    ax.plot([r_xy[0], b_xy[0]], [r_xy[1], b_xy[1]], 'k--', alpha=0.3, linewidth=1)

    # D65 reference
    d65_xy = (0.3127, 0.3290)
    ax.plot(*d65_xy, '*', color='black', markersize=12)
    ax.annotate('D65\n(6500K)', xy=d65_xy, xytext=(d65_xy[0]-0.08, d65_xy[1]-0.05),
               fontsize=9)

    # Styling
    ax.set_xlim(0.0, 0.75)
    ax.set_ylim(0.0, 0.85)
    ax.set_xlabel('x chromaticity', fontsize=12)
    ax.set_ylabel('y chromaticity', fontsize=12)
    ax.set_title('RGBG Color Modes: Chromaticity Positions and PWM Settings', fontsize=14)
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    return fig


def planckian_locus_xy(T):
    """Calculate xy chromaticity for blackbody at temperature T (Kelvin)."""
    # Approximation valid for 1667K to 25000K
    if T < 4000:
        x = (-0.2661239e9/T**3 - 0.2343589e6/T**2 + 0.8776956e3/T + 0.179910)
    else:
        x = (-3.0258469e9/T**3 + 2.1070379e6/T**2 + 0.2226347e3/T + 0.240390)

    if T < 2222:
        y = (-1.1063814*x**3 - 1.34811020*x**2 + 2.18555832*x - 0.20219683)
    elif T < 4000:
        y = (-0.9549476*x**3 - 1.37418593*x**2 + 2.09137015*x - 0.16748867)
    else:
        y = (3.0817580*x**3 - 5.87338670*x**2 + 3.75112997*x - 0.37001483)

    return x, y


def plot_planckian_locus():
    """
    Show Planckian locus (blackbody curve) with CCT markers.
    """
    fig, ax = plt.subplots(figsize=(12, 10))

    # Calculate spectral locus
    x_locus, y_locus = calculate_spectral_locus()

    # Draw spectral locus with colors
    for i in range(len(CIE_WAVELENGTHS) - 1):
        if i < len(x_locus) - 1:
            wl = CIE_WAVELENGTHS[i]
            color = wavelength_to_rgb(wl)
            ax.plot([x_locus[i], x_locus[i+1]], [y_locus[i], y_locus[i+1]],
                   color=color, linewidth=3, alpha=0.5)

    # Purple line
    ax.plot([x_locus[-1], x_locus[0]], [y_locus[-1], y_locus[0]],
           color='purple', linewidth=2, linestyle='--', alpha=0.3)

    # Calculate Planckian locus
    temps = np.linspace(1800, 10000, 200)
    planck_x = []
    planck_y = []
    for T in temps:
        x, y = planckian_locus_xy(T)
        planck_x.append(x)
        planck_y.append(y)

    # Draw Planckian locus
    ax.plot(planck_x, planck_y, 'k-', linewidth=3, label='Planckian Locus')

    # Mark specific CCTs
    cct_markers = [
        (2700, 'Warm White\n(Incandescent)', '#FFB366'),
        (3000, '3000K', '#FFC080'),
        (4000, 'Neutral\n4000K', '#FFE0B0'),
        (5000, 'Daylight\n5000K', '#FFF0E0'),
        (5500, '5500K', '#FFF4E8'),
        (6500, 'D65\n6500K', '#FFFFFF'),
        (8000, 'Cool\n8000K', '#E8F0FF'),
    ]

    for T, label, color in cct_markers:
        x, y = planckian_locus_xy(T)
        ax.plot(x, y, 'o', color=color, markersize=15,
               markeredgecolor='black', markeredgewidth=2)

        # Alternate label positions
        if T < 4000:
            offset = (0.02, 0.02)
        elif T < 6000:
            offset = (0.02, -0.03)
        else:
            offset = (-0.02, -0.04)

        ax.annotate(label, xy=(x, y), xytext=(x+offset[0], y+offset[1]),
                   fontsize=9, ha='center',
                   bbox=dict(boxstyle='round,pad=0.2', facecolor=color,
                            edgecolor='black', alpha=0.9))

    # Draw Duv lines (iso-temperature lines perpendicular to locus)
    ax.annotate('Duv > 0\n(greenish)', xy=(0.35, 0.40), fontsize=10, color='green',
               ha='center')
    ax.annotate('Duv < 0\n(pinkish)', xy=(0.35, 0.32), fontsize=10, color='#CC6699',
               ha='center')

    # Add arrow showing Duv direction
    ax.annotate('', xy=(0.35, 0.38), xytext=(0.35, 0.35),
               arrowprops=dict(arrowstyle='->', color='gray', lw=2))

    # Styling
    ax.set_xlim(0.2, 0.55)
    ax.set_ylim(0.25, 0.45)
    ax.set_xlabel('x chromaticity', fontsize=12)
    ax.set_ylabel('y chromaticity', fontsize=12)
    ax.set_title('Planckian Locus: Correlated Color Temperature (CCT)', fontsize=14)
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)
    ax.legend(loc='upper right')

    plt.tight_layout()
    return fig


def plot_scale_derivation():
    """
    Show how different scale values affect the resulting chromaticity.
    """
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))

    # Get LED XYZ values
    red = LED_DATABASE['XPE2_RED']
    green = LED_DATABASE['XPE2_GREEN']
    blue = LED_DATABASE['XPE2_BLUE']

    r_xyz = led_to_xyz(red)
    g_xyz = led_to_xyz(green)
    b_xyz = led_to_xyz(blue)

    r_xy = xyz_to_chromaticity(*r_xyz)
    g_xy = xyz_to_chromaticity(*g_xyz)
    b_xy = xyz_to_chromaticity(*b_xyz)

    # ===== Panel 1: R+G,G blend (Yellow mode) =====
    ax1 = axes[0]

    # Draw RGB triangle
    triangle = Polygon([r_xy, g_xy, b_xy], closed=True, fill=True,
                      facecolor='#f8f8f8', edgecolor='gray', linewidth=1, alpha=0.5)
    ax1.add_patch(triangle)

    # Plot LEDs
    ax1.plot(*r_xy, 'o', color='#CC0000', markersize=15, markeredgecolor='black')
    ax1.plot(*g_xy, 'o', color='#00CC00', markersize=15, markeredgecolor='black')

    # Calculate and plot trajectory for different scales
    scales = [16, 32, 64, 96, 128, 192, 255]
    trajectory_x = []
    trajectory_y = []

    for scale in scales:
        s = scale / 256
        X = r_xyz[0] + 2*s*g_xyz[0]
        Y = r_xyz[1] + 2*s*g_xyz[1]
        Z = r_xyz[2] + 2*s*g_xyz[2]
        xy = xyz_to_chromaticity(X, Y, Z)
        trajectory_x.append(xy[0])
        trajectory_y.append(xy[1])

        # Color gradient from red to yellow-green
        color = plt.cm.YlOrRd(1 - s)
        ax1.plot(*xy, 's', color=color, markersize=12, markeredgecolor='black')
        ax1.annotate(f'{scale}', xy=xy, xytext=(xy[0]+0.01, xy[1]+0.01),
                    fontsize=8)

    # Draw trajectory line
    ax1.plot(trajectory_x, trajectory_y, 'k--', alpha=0.5, linewidth=2)

    # Highlight chosen value
    chosen_scale = 32
    s = chosen_scale / 256
    X = r_xyz[0] + 2*s*g_xyz[0]
    Y = r_xyz[1] + 2*s*g_xyz[1]
    Z = r_xyz[2] + 2*s*g_xyz[2]
    chosen_xy = xyz_to_chromaticity(X, Y, Z)
    ax1.plot(*chosen_xy, 's', color='#FFAA00', markersize=18,
            markeredgecolor='black', markeredgewidth=3)
    ax1.annotate(f'Chosen: {chosen_scale}\nxy=({chosen_xy[0]:.3f}, {chosen_xy[1]:.3f})',
                xy=chosen_xy, xytext=(chosen_xy[0]-0.1, chosen_xy[1]+0.05),
                fontsize=10, fontweight='bold',
                bbox=dict(boxstyle='round', facecolor='#FFAA00', alpha=0.8),
                arrowprops=dict(arrowstyle='->', color='black'))

    ax1.set_xlim(0.15, 0.75)
    ax1.set_ylim(0.25, 0.80)
    ax1.set_xlabel('x chromaticity', fontsize=11)
    ax1.set_ylabel('y chromaticity', fontsize=11)
    ax1.set_title('Yellow Mode: R + G×scale (MAIN2_SCALE_RGG)', fontsize=12)
    ax1.set_aspect('equal')
    ax1.grid(True, alpha=0.3)

    # ===== Panel 2: G,G+B blend (Cyan mode) =====
    ax2 = axes[1]

    # Draw RGB triangle
    triangle = Polygon([r_xy, g_xy, b_xy], closed=True, fill=True,
                      facecolor='#f8f8f8', edgecolor='gray', linewidth=1, alpha=0.5)
    ax2.add_patch(triangle)

    # Plot LEDs
    ax2.plot(*g_xy, 'o', color='#00CC00', markersize=15, markeredgecolor='black')
    ax2.plot(*b_xy, 'o', color='#0066FF', markersize=15, markeredgecolor='black')

    # Calculate and plot trajectory for different scales
    trajectory_x = []
    trajectory_y = []

    for scale in scales:
        s = scale / 256
        X = 2*s*g_xyz[0] + b_xyz[0]
        Y = 2*s*g_xyz[1] + b_xyz[1]
        Z = 2*s*g_xyz[2] + b_xyz[2]
        xy = xyz_to_chromaticity(X, Y, Z)
        trajectory_x.append(xy[0])
        trajectory_y.append(xy[1])

        # Color gradient from blue to cyan-green
        color = plt.cm.GnBu(1 - s*0.7)
        ax2.plot(*xy, 's', color=color, markersize=12, markeredgecolor='black')
        ax2.annotate(f'{scale}', xy=xy, xytext=(xy[0]+0.01, xy[1]+0.01),
                    fontsize=8)

    # Draw trajectory line
    ax2.plot(trajectory_x, trajectory_y, 'k--', alpha=0.5, linewidth=2)

    # Highlight chosen value
    chosen_scale = 128
    s = chosen_scale / 256
    X = 2*s*g_xyz[0] + b_xyz[0]
    Y = 2*s*g_xyz[1] + b_xyz[1]
    Z = 2*s*g_xyz[2] + b_xyz[2]
    chosen_xy = xyz_to_chromaticity(X, Y, Z)
    ax2.plot(*chosen_xy, 's', color='#00CCCC', markersize=18,
            markeredgecolor='black', markeredgewidth=3)
    ax2.annotate(f'Chosen: {chosen_scale}\nxy=({chosen_xy[0]:.3f}, {chosen_xy[1]:.3f})',
                xy=chosen_xy, xytext=(chosen_xy[0]+0.03, chosen_xy[1]+0.08),
                fontsize=10, fontweight='bold',
                bbox=dict(boxstyle='round', facecolor='#00CCCC', alpha=0.8),
                arrowprops=dict(arrowstyle='->', color='black'))

    ax2.set_xlim(0.10, 0.20)
    ax2.set_ylim(0.0, 0.80)
    ax2.set_xlabel('x chromaticity', fontsize=11)
    ax2.set_ylabel('y chromaticity', fontsize=11)
    ax2.set_title('Cyan Mode: G×scale + B (MAIN2_SCALE_GGB)', fontsize=12)
    ax2.set_aspect('equal')
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    return fig


def main():
    """Generate all figures."""
    # Ensure output directory exists
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    print(f"Generating figures in {OUTPUT_DIR}")

    # Generate each figure
    figures = [
        ('cmf_spectrum.png', plot_cmf_with_spectrum, 'CIE CMF with spectrum'),
        ('chromaticity_diagram.png', plot_chromaticity_diagram, 'Chromaticity diagram'),
        ('led_spd_comparison.png', plot_led_spd_comparison, 'LED SPD comparison'),
        ('rgbg_mixing.png', plot_rgbg_mixing, 'RGBG mixing demonstration'),
        ('spectrum_bar.png', plot_spectrum_bar, 'Spectrum reference bar'),
        ('color_modes.png', plot_color_modes, 'Color modes chromaticity map'),
        ('planckian_locus.png', plot_planckian_locus, 'Planckian locus (CCT)'),
        ('scale_derivation.png', plot_scale_derivation, 'Scale factor derivation'),
    ]

    for filename, plot_func, description in figures:
        print(f"  Generating {filename}... ", end='')
        fig = plot_func()
        filepath = OUTPUT_DIR / filename
        fig.savefig(filepath, dpi=150, bbox_inches='tight',
                   facecolor='white', edgecolor='none')
        plt.close(fig)
        print(f"done ({filepath})")

    print(f"\nAll figures generated in {OUTPUT_DIR}")
    print("Update docs/color-science.md to reference these images.")


if __name__ == '__main__':
    main()
