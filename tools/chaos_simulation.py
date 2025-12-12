#!/usr/bin/env python3
"""
Chaotic Pendulum Color Animation - Simulation and Visualization

This script simulates the exact algorithm implemented in the D4K-3ch firmware
and generates visualizations for documentation.

The algorithm uses coupled oscillators to create organic, non-repeating
color patterns that roam around white with excursions to saturated colors.
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib.colors import hsv_to_rgb
import colorsys

# Match the firmware constants exactly
def triangle_wave(phase):
    """Triangle wave: 0→255→0 as phase goes 0→128→255"""
    phase = phase & 0xFF
    if phase < 128:
        return phase * 2
    else:
        return 255 - (phase - 128) * 2

def signed_wave(phase):
    """Convert triangle wave to signed (-128 to 127)"""
    return triangle_wave(phase) - 128

# Global frame counter for driving oscillation
_chaos_frame = 0

def chaos_update(state, energy=128):
    """
    Exact implementation of the firmware chaos_update() function.

    Parameters:
    - state: dict with theta1, omega1, theta2, omega2 (all int16)
    - energy: 0-255, controls animation speed

    Returns updated state dict.
    """
    global _chaos_frame

    theta1 = state['theta1']
    omega1 = state['omega1']
    theta2 = state['theta2']
    omega2 = state['omega2']

    # Speed scale: 4-19 based on energy
    scale = 4 + (energy >> 4)

    # Use triangle wave as sinusoidal approximation
    wave1 = signed_wave((theta1 >> 8) & 0xFF)
    wave2 = signed_wave((theta2 >> 8) & 0xFF)
    wave_diff = signed_wave(((theta2 - theta1) >> 8) & 0xFF)

    # Driving force: periodic kick to sustain oscillations
    # This creates a driven double pendulum which exhibits true chaos
    _chaos_frame = (_chaos_frame + 1) & 0xFF
    drive = signed_wave((_chaos_frame * 3) & 0xFF)
    drive_force = (drive * (32 + (energy >> 3))) >> 7

    # Coupled pendulum acceleration with driving
    acc1 = ((-20 * wave1) >> 4) + ((16 * wave_diff) >> 4) + drive_force
    acc2 = ((-16 * wave2) >> 4) - ((16 * wave_diff) >> 4)

    # Very light damping (prevents runaway but allows sustained motion)
    omega1 = omega1 - (omega1 >> 9) + acc1
    omega2 = omega2 - (omega2 >> 9) + acc2

    # Velocity limits
    omega1 = max(-3000, min(3000, omega1))
    omega2 = max(-2500, min(2500, omega2))

    # Position update (scaled by energy)
    theta1 += (omega1 * scale) >> 4
    theta2 += (omega2 * scale) >> 4

    # Wrap to int16 range (simulating overflow)
    theta1 = ((theta1 + 32768) % 65536) - 32768
    theta2 = ((theta2 + 32768) % 65536) - 32768

    return {
        'theta1': theta1,
        'omega1': omega1,
        'theta2': theta2,
        'omega2': omega2
    }

def state_to_color(state):
    """Convert chaos state to HSV color (matching firmware)."""
    # Hue from theta1 (full 0-255 range)
    hue = (state['theta1'] >> 8) & 0xFF

    # Saturation from theta2 (centered at 140, range 60-220)
    sat_offset = ((state['theta2'] >> 9) + 128) % 256 - 128  # signed byte
    sat = 140 + sat_offset
    sat = max(60, min(220, sat))

    return hue, sat

def simulate_chaos(n_steps, energy=128, initial_state=None):
    """Run chaos simulation for n_steps."""
    global _chaos_frame
    _chaos_frame = 0  # Reset frame counter for each simulation

    if initial_state is None:
        state = {
            'theta1': 0,
            'omega1': 200,
            'theta2': 0,
            'omega2': 150
        }
    else:
        state = initial_state.copy()

    history = {
        'theta1': [], 'omega1': [],
        'theta2': [], 'omega2': [],
        'hue': [], 'sat': []
    }

    for _ in range(n_steps):
        history['theta1'].append(state['theta1'])
        history['omega1'].append(state['omega1'])
        history['theta2'].append(state['theta2'])
        history['omega2'].append(state['omega2'])

        hue, sat = state_to_color(state)
        history['hue'].append(hue)
        history['sat'].append(sat)

        state = chaos_update(state, energy)

    return {k: np.array(v) for k, v in history.items()}


def plot_phase_space(history, filename):
    """Create phase space diagram showing chaotic attractor."""
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))

    # Oscillator 1 phase space (theta1 vs omega1)
    ax1 = axes[0]
    # Color by time
    points = np.array([history['theta1'], history['omega1']]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)

    # Use hue for coloring
    colors = plt.cm.hsv(history['hue'][:-1] / 255.0)
    lc = LineCollection(segments, colors=colors, linewidth=0.5, alpha=0.7)
    ax1.add_collection(lc)
    ax1.autoscale()
    ax1.set_xlabel('θ₁ (Hue Position)', fontsize=12)
    ax1.set_ylabel('ω₁ (Hue Velocity)', fontsize=12)
    ax1.set_title('Oscillator 1: Hue Dynamics', fontsize=14)
    ax1.grid(True, alpha=0.3)

    # Oscillator 2 phase space (theta2 vs omega2)
    ax2 = axes[1]
    points = np.array([history['theta2'], history['omega2']]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)

    # Color by saturation
    sat_norm = (history['sat'][:-1] - 60) / 160.0
    colors = plt.cm.viridis(sat_norm)
    lc = LineCollection(segments, colors=colors, linewidth=0.5, alpha=0.7)
    ax2.add_collection(lc)
    ax2.autoscale()
    ax2.set_xlabel('θ₂ (Saturation Position)', fontsize=12)
    ax2.set_ylabel('ω₂ (Saturation Velocity)', fontsize=12)
    ax2.set_title('Oscillator 2: Saturation Dynamics', fontsize=14)
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"Saved: {filename}")


def plot_color_trajectory(history, filename):
    """Create color space trajectory showing HSV path."""
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))

    # Left: Polar plot of hue/saturation
    ax1 = axes[0]
    ax1 = plt.subplot(121, projection='polar')

    # Convert hue to radians, saturation to radius
    theta = history['hue'] * 2 * np.pi / 255.0
    r = history['sat']

    # Plot trajectory colored by actual color
    for i in range(len(theta) - 1):
        h = history['hue'][i] / 255.0
        s = history['sat'][i] / 255.0
        rgb = colorsys.hsv_to_rgb(h, s, 0.9)
        ax1.plot(theta[i:i+2], r[i:i+2], color=rgb, linewidth=0.5, alpha=0.7)

    ax1.set_ylim(0, 255)
    ax1.set_title('Color Trajectory in HSV Space\n(radius = saturation, angle = hue)', fontsize=12)
    ax1.set_rticks([60, 100, 140, 180, 220])
    ax1.set_rlabel_position(45)

    # Right: Hue vs Saturation scatter with density
    ax2 = axes[1]

    # Create 2D histogram for density
    h_bins = np.linspace(0, 255, 64)
    s_bins = np.linspace(60, 220, 40)
    H, xedges, yedges = np.histogram2d(history['hue'], history['sat'], bins=[h_bins, s_bins])

    # Plot as heatmap
    extent = [0, 255, 60, 220]
    im = ax2.imshow(H.T, origin='lower', extent=extent, aspect='auto', cmap='hot')
    plt.colorbar(im, ax=ax2, label='Visit Frequency')

    # Overlay white region indicator
    ax2.axhline(y=140, color='white', linestyle='--', linewidth=1, alpha=0.7, label='White center (sat=140)')

    ax2.set_xlabel('Hue (0-255)', fontsize=12)
    ax2.set_ylabel('Saturation (60-220)', fontsize=12)
    ax2.set_title('Color Space Density\n(brighter = more time spent)', fontsize=12)
    ax2.legend(loc='upper right')

    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"Saved: {filename}")


def plot_time_series(history, filename, n_points=2000):
    """Create time series plot showing color evolution."""
    fig, axes = plt.subplots(4, 1, figsize=(14, 10), sharex=True)

    t = np.arange(n_points) * 16 / 1000  # Convert to seconds (16ms per tick)

    # Hue over time
    ax1 = axes[0]
    for i in range(n_points - 1):
        h = history['hue'][i] / 255.0
        s = history['sat'][i] / 255.0
        rgb = colorsys.hsv_to_rgb(h, s, 0.9)
        ax1.axvspan(t[i], t[i+1], color=rgb, alpha=1.0)
    ax1.set_ylabel('Color', fontsize=12)
    ax1.set_title('Actual Color Output Over Time', fontsize=14)
    ax1.set_yticks([])

    # Hue value
    ax2 = axes[1]
    ax2.plot(t, history['hue'][:n_points], 'b-', linewidth=0.5)
    ax2.set_ylabel('Hue', fontsize=12)
    ax2.set_ylim(0, 255)
    ax2.grid(True, alpha=0.3)
    ax2.axhline(y=0, color='r', linestyle='-', linewidth=0.5, alpha=0.5)
    ax2.axhline(y=85, color='g', linestyle='-', linewidth=0.5, alpha=0.5)
    ax2.axhline(y=170, color='b', linestyle='-', linewidth=0.5, alpha=0.5)

    # Saturation value
    ax3 = axes[2]
    ax3.plot(t, history['sat'][:n_points], 'purple', linewidth=0.5)
    ax3.axhline(y=140, color='gray', linestyle='--', linewidth=1, alpha=0.7, label='White center')
    ax3.fill_between(t, 60, 220, alpha=0.1, color='gray')
    ax3.set_ylabel('Saturation', fontsize=12)
    ax3.set_ylim(40, 240)
    ax3.grid(True, alpha=0.3)
    ax3.legend(loc='upper right')

    # Angular velocities (showing chaos)
    ax4 = axes[3]
    ax4.plot(t, history['omega1'][:n_points], 'b-', linewidth=0.5, alpha=0.7, label='ω₁ (hue)')
    ax4.plot(t, history['omega2'][:n_points], 'r-', linewidth=0.5, alpha=0.7, label='ω₂ (sat)')
    ax4.set_ylabel('Angular Velocity', fontsize=12)
    ax4.set_xlabel('Time (seconds)', fontsize=12)
    ax4.grid(True, alpha=0.3)
    ax4.legend(loc='upper right')

    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"Saved: {filename}")


def plot_energy_comparison(filename):
    """Compare chaos behavior at different energy levels."""
    fig, axes = plt.subplots(3, 3, figsize=(14, 12))

    energies = [32, 128, 224]  # Low, medium, high
    labels = ['Low Energy (32)', 'Medium Energy (128)', 'High Energy (224)']

    n_steps = 5000

    for col, (energy, label) in enumerate(zip(energies, labels)):
        history = simulate_chaos(n_steps, energy=energy)

        # Phase space
        ax1 = axes[0, col]
        ax1.plot(history['theta1'], history['omega1'], 'b-', linewidth=0.2, alpha=0.5)
        ax1.set_title(f'{label}\nPhase Space', fontsize=11)
        ax1.set_xlabel('θ₁')
        ax1.set_ylabel('ω₁')
        ax1.grid(True, alpha=0.3)

        # Color bar (first 1000 steps)
        ax2 = axes[1, col]
        for i in range(min(500, n_steps - 1)):
            h = history['hue'][i] / 255.0
            s = history['sat'][i] / 255.0
            rgb = colorsys.hsv_to_rgb(h, s, 0.9)
            ax2.axvspan(i, i+1, color=rgb, alpha=1.0)
        ax2.set_title('Color Evolution', fontsize=11)
        ax2.set_xlabel('Frame')
        ax2.set_yticks([])

        # Hue histogram
        ax3 = axes[2, col]
        ax3.hist(history['hue'], bins=64, density=True, alpha=0.7, color='blue')
        ax3.set_title('Hue Distribution', fontsize=11)
        ax3.set_xlabel('Hue')
        ax3.set_ylabel('Density')

    plt.suptitle('Effect of Energy Parameter on Chaos Behavior', fontsize=14, y=1.02)
    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"Saved: {filename}")


def plot_coupling_diagram(filename):
    """Diagram showing the coupled oscillator structure."""
    fig, ax = plt.subplots(1, 1, figsize=(10, 8))

    # Draw oscillator circles
    circle1 = plt.Circle((0.3, 0.55), 0.15, fill=False, linewidth=3, color='blue')
    circle2 = plt.Circle((0.7, 0.55), 0.15, fill=False, linewidth=3, color='red')
    ax.add_patch(circle1)
    ax.add_patch(circle2)

    # Labels
    ax.text(0.3, 0.55, 'θ₁, ω₁\n(Hue)', ha='center', va='center', fontsize=14, fontweight='bold')
    ax.text(0.7, 0.55, 'θ₂, ω₂\n(Sat)', ha='center', va='center', fontsize=14, fontweight='bold')

    # Coupling arrows
    ax.annotate('', xy=(0.55, 0.60), xytext=(0.45, 0.60),
                arrowprops=dict(arrowstyle='->', color='green', lw=2))
    ax.annotate('', xy=(0.45, 0.50), xytext=(0.55, 0.50),
                arrowprops=dict(arrowstyle='->', color='green', lw=2))
    ax.text(0.5, 0.67, 'Coupling', ha='center', fontsize=11, color='green')

    # Driving force (new)
    ax.annotate('', xy=(0.3, 0.75), xytext=(0.3, 0.85),
                arrowprops=dict(arrowstyle='->', color='orange', lw=3))
    ax.text(0.3, 0.90, 'Driving Force\nwave(frame×3)', ha='center', fontsize=11, color='orange', fontweight='bold')

    # Self-restoring force arrows
    ax.annotate('', xy=(0.15, 0.55), xytext=(0.05, 0.55),
                arrowprops=dict(arrowstyle='->', color='purple', lw=2))
    ax.text(0.05, 0.45, 'Restoring\nForce', ha='center', fontsize=10, color='purple')

    ax.annotate('', xy=(0.85, 0.55), xytext=(0.95, 0.55),
                arrowprops=dict(arrowstyle='->', color='purple', lw=2))
    ax.text(0.95, 0.45, 'Restoring\nForce', ha='center', fontsize=10, color='purple')

    # Output arrows
    ax.annotate('', xy=(0.3, 0.25), xytext=(0.3, 0.40),
                arrowprops=dict(arrowstyle='->', color='blue', lw=2))
    ax.text(0.3, 0.17, 'Hue\n(0-255)', ha='center', fontsize=12, color='blue')

    ax.annotate('', xy=(0.7, 0.25), xytext=(0.7, 0.40),
                arrowprops=dict(arrowstyle='->', color='red', lw=2))
    ax.text(0.7, 0.17, 'Saturation\n(60-220)', ha='center', fontsize=12, color='red')

    # Damping annotation
    ax.text(0.5, 0.78, 'Light Damping: ω ← ω - ω/512', ha='center', fontsize=10,
            style='italic', color='gray')

    # Equations
    eq_text = """Driven Coupled Pendulum Equations:

drive_force = wave(frame×3) · (32 + energy/8) / 128

acc₁ = -20·wave(θ₁)/16 + 16·wave(θ₂-θ₁)/16 + drive_force
acc₂ = -16·wave(θ₂)/16 - 16·wave(θ₂-θ₁)/16

ω₁ ← ω₁ - ω₁/512 + acc₁    (velocity update)
ω₂ ← ω₂ - ω₂/512 + acc₂

θ₁ ← θ₁ + ω₁·scale/16      (position update)
θ₂ ← θ₂ + ω₂·scale/16"""

    ax.text(0.5, 0.02, eq_text, ha='center', va='bottom', fontsize=9,
            family='monospace', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.set_aspect('equal')
    ax.axis('off')
    ax.set_title('Driven Coupled Oscillator Architecture', fontsize=16, fontweight='bold', pad=20)

    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"Saved: {filename}")


def plot_rgb_output(history, filename, n_points=500):
    """Show actual RGB channel outputs over time."""
    fig, axes = plt.subplots(4, 1, figsize=(14, 10), sharex=True)

    t = np.arange(n_points) * 16 / 1000  # Convert to seconds

    # Convert HSV to RGB for each point
    r_vals, g_vals, b_vals = [], [], []
    for i in range(n_points):
        h = history['hue'][i] / 255.0
        s = history['sat'][i] / 255.0
        r, g, b = colorsys.hsv_to_rgb(h, s, 1.0)
        r_vals.append(r * 255)
        g_vals.append(g * 255)
        b_vals.append(b * 255)

    r_vals = np.array(r_vals)
    g_vals = np.array(g_vals)
    b_vals = np.array(b_vals)

    # Combined color bar
    ax1 = axes[0]
    for i in range(n_points - 1):
        rgb = (r_vals[i]/255, g_vals[i]/255, b_vals[i]/255)
        ax1.axvspan(t[i], t[i+1], color=rgb, alpha=1.0)
    ax1.set_ylabel('Color', fontsize=12)
    ax1.set_title('RGB LED Output Over Time', fontsize=14)
    ax1.set_yticks([])

    # Individual channels
    ax2 = axes[1]
    ax2.fill_between(t, 0, r_vals, color='red', alpha=0.7)
    ax2.set_ylabel('Red (LED4)', fontsize=12)
    ax2.set_ylim(0, 260)
    ax2.grid(True, alpha=0.3)

    ax3 = axes[2]
    ax3.fill_between(t, 0, g_vals, color='green', alpha=0.7)
    ax3.set_ylabel('Green (LED3)', fontsize=12)
    ax3.set_ylim(0, 260)
    ax3.grid(True, alpha=0.3)

    ax4 = axes[3]
    ax4.fill_between(t, 0, b_vals, color='blue', alpha=0.7)
    ax4.set_ylabel('Blue (Main2)', fontsize=12)
    ax4.set_xlabel('Time (seconds)', fontsize=12)
    ax4.set_ylim(0, 260)
    ax4.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"Saved: {filename}")


def main():
    """Generate all visualizations."""
    print("Simulating chaotic pendulum dynamics...")

    # Long simulation for phase space
    history_long = simulate_chaos(50000, energy=128)

    # Shorter simulation for time series
    history_short = simulate_chaos(3000, energy=128)

    print("\nGenerating visualizations...")

    # Create output directory
    import os
    os.makedirs('docs/images', exist_ok=True)

    # Generate all plots
    plot_coupling_diagram('docs/images/chaos-coupling-diagram.png')
    plot_phase_space(history_long, 'docs/images/chaos-phase-space.png')
    plot_color_trajectory(history_long, 'docs/images/chaos-color-trajectory.png')
    plot_time_series(history_short, 'docs/images/chaos-time-series.png')
    plot_energy_comparison('docs/images/chaos-energy-comparison.png')
    plot_rgb_output(history_short, 'docs/images/chaos-rgb-output.png')

    print("\nAll visualizations generated successfully!")
    print("\nStatistics:")
    print(f"  Hue range: {history_long['hue'].min():.0f} - {history_long['hue'].max():.0f}")
    print(f"  Saturation range: {history_long['sat'].min():.0f} - {history_long['sat'].max():.0f}")
    print(f"  Mean saturation: {history_long['sat'].mean():.1f} (target: 140)")
    print(f"  Omega1 range: {history_long['omega1'].min():.0f} - {history_long['omega1'].max():.0f}")
    print(f"  Omega2 range: {history_long['omega2'].min():.0f} - {history_long['omega2'].max():.0f}")


if __name__ == '__main__':
    main()
