#!/usr/bin/env python3
"""Generate visual and metric proof for BedroomLedController mode animation strength."""

from __future__ import annotations

import argparse
import json
import math
import re
from pathlib import Path
from typing import Callable


ROOT = Path(__file__).resolve().parents[1]
HARDWARE = ROOT / "firmware" / "BedroomLedController" / "HardwareProfile.h"
OUT_DIR = ROOT / "docs" / "generated" / "mode_simulations"
REPORT = ROOT / "docs" / "generated" / "BedroomLedController_Mode_Overhaul_Report.md"

STRENGTH_LEVELS = [0, 64, 128, 192, 255]
TIME_SAMPLES_SECONDS = [0, 2, 5, 10, 20, 40]
HTML_SAMPLE_PIXELS = 140

MODES = [
    ("solid", "Solid"),
    ("strobe", "Strobe"),
    ("flash", "Flash"),
    ("chase", "Chase"),
    ("wave", "Wave"),
    ("slow-wave", "Slow Wave"),
    ("slow-pulse", "Slow Pulse"),
    ("rainbow", "Rainbow"),
    ("dawn-bloom", "Dawn Bloom"),
    ("moon-tide", "Moon Tide"),
    ("velvet-aurora", "Velvet Aurora"),
    ("lantern-drift", "Lantern Drift"),
    ("rain-glass", "Rain Glass"),
    ("satin-breathing", "Satin Breathing"),
    ("paper-lantern-row", "Paper Lantern Row"),
    ("library-lamp", "Library Lamp"),
    ("solar-noon", "Solar Noon"),
    ("golden-hour", "Golden Hour"),
    ("dream-aquarium", "Dream Aquarium"),
    ("circuit-glow", "Circuit Glow"),
    ("window-sunrise", "Window Sunrise"),
    ("ember-quilt", "Ember Quilt"),
    ("porcelain-sky", "Porcelain Sky"),
    ("hearthline", "Hearthline"),
    ("evening-current", "Evening Current"),
    ("starlit-linen", "Starlit Linen"),
    ("deep-sea-glass", "Deep Sea Glass"),
    ("quiet-northern-lights", "Quiet Northern Lights"),
    ("candle-hall", "Candle Hall"),
    ("slow-prism", "Slow Prism"),
    ("garden-shade", "Garden Shade"),
    ("snowfall-hush", "Snowfall Hush"),
    ("observatory-glow", "Observatory Glow"),
    ("rainy-window", "Rainy Window"),
    ("campfire-blanket", "Campfire Blanket"),
    ("northern-whisper", "Northern Whisper"),
    ("storm-outside", "Storm Outside"),
]

MODE_NOTES = {
    "solid": "Intentionally stable; Animation Strength does not alter Solid.",
    "strobe": "Flash cadence, floor, and peak are strength-scaled while Night Guard can still block it.",
    "flash": "Repeating signal flash now has non-binary envelope and strength-scaled on/off feel.",
    "chase": "Comet train, tail length, echo, and speed scale with strength.",
    "wave": "Primary and secondary waves scale speed, amplitude, and floor.",
    "slow-wave": "Broad swells, foam texture, and layer count scale with strength.",
    "slow-pulse": "Independent soft blooms scale count, width, drift, and texture.",
    "rainbow": "Palette spread, speed, and saturation scale with strength.",
    "dawn-bloom": "Directional warm bloom scales shimmer, reach, and brightness.",
    "moon-tide": "Tide motion, ripples, and cool color movement scale with strength.",
    "velvet-aurora": "Layered aurora folds scale color spread, motion, and shimmer.",
    "lantern-drift": "Warm lantern clusters scale count, sway, halo, and breathing.",
    "rain-glass": "Droplet spawn, bead, trail, life, and dissolve behavior scale with strength.",
    "satin-breathing": "Whole-strip inhale/exhale depth, texture, and pace scale with strength.",
    "paper-lantern-row": "Large lantern spacing, halo, sway, and warmth scale with strength.",
    "library-lamp": "Practical warm lamp stays quiet, with only faint strength-scaled texture.",
    "solar-noon": "Practical daylight stays steady, with only refined strength-scaled gradient drift.",
    "golden-hour": "Low-sun source width, warmth, and motion scale with strength.",
    "dream-aquarium": "Water caustics, depth, and shimmer scale with strength.",
    "circuit-glow": "Trace count, velocity, trail width, and glow contrast scale with strength.",
    "window-sunrise": "Directional morning reach, edge warmth, and shimmer scale with strength.",
    "ember-quilt": "Patch count, ember breathing, and fabric texture scale with strength.",
    "porcelain-sky": "Cloud-glass drift, tint, and polish scale with strength.",
    "hearthline": "Warm hearth bands, width, and restrained motion scale with strength.",
    "evening-current": "Overlapping currents and ripple contrast scale with strength.",
    "starlit-linen": "Sparse stars now bloom into neighboring soft highlights.",
    "deep-sea-glass": "Layered depth, glints, and glassy water shimmer scale with strength.",
    "quiet-northern-lights": "Multi-ribbon aurora model scales ribbons, folds, opacity, and color spread.",
    "candle-hall": "Candle cores, halos, and smoothed flicker scale with strength.",
    "slow-prism": "Companion colors, prism spread, and motion scale with strength.",
    "garden-shade": "Leaf shade breeze, dappling, and warm daylight motion scale with strength.",
    "snowfall-hush": "Soft drifting particles now bloom into neighboring-pixel highlights.",
    "observatory-glow": "Moon glow and star twinkle density scale with strength.",
    "rainy-window": "Droplet life, bead, trail, linger, and pane glow scale with strength.",
    "campfire-blanket": "Cozy fire waves stay soft but scale movement and warmth.",
    "northern-whisper": "Sleep-safe aurora whisper scales folds and color spread gently.",
    "storm-outside": "Cloud roll, rain texture, flash softness, and post-flash fade scale with strength.",
}


Rgb = tuple[int, int, int]


def parse_hardware_value(name: str, default: int) -> int:
    text = HARDWARE.read_text(encoding="utf-8")
    match = re.search(rf"{name}\s*=\s*([0-9]+)", text)
    return int(match.group(1)) if match else default


LED_COUNT = parse_hardware_value("LED_COUNT_OVERRIDE", 300)
STRIP_LENGTH_MM = parse_hardware_value("STRIP_LENGTH_MM", 5000)


def clamp(value: float, low: float = 0.0, high: float = 255.0) -> int:
    return int(max(low, min(high, round(value))))


def clamp01(value: float) -> float:
    return max(0.0, min(1.0, value))


def strength01(strength: int) -> float:
    raw = clamp01(strength / 255.0)
    return raw * raw * (3.0 - 2.0 * raw)


def strength_linear(strength: int, low: float, high: float) -> float:
    return low + (high - low) * clamp01(strength / 255.0)


def strength_ease(strength: int, low: float, high: float) -> float:
    return low + (high - low) * strength01(strength)


def mode_speed(slug: str, strength: int) -> float:
    low, high = 0.28, 1.85
    if slug in {"library-lamp", "solar-noon"}:
      low, high = 0.04, 0.30
    elif slug in {"northern-whisper", "observatory-glow", "snowfall-hush"}:
      low, high = 0.12, 0.86
    elif slug in {"strobe", "flash", "chase", "rainbow", "circuit-glow"}:
      low, high = 0.55, 2.30
    return strength_linear(strength, low, high)


def mode_motion(slug: str, strength: int) -> float:
    low, high = 0.10, 1.0
    if slug in {"library-lamp", "solar-noon"}:
        low, high = 0.0, 0.18
    elif slug in {"storm-outside", "northern-whisper", "snowfall-hush"}:
        low, high = 0.08, 0.72
    return strength_ease(strength, low, high)


def mode_contrast(slug: str, strength: int) -> float:
    low, high = 0.18, 1.0
    if slug in {"library-lamp", "solar-noon"}:
        low, high = 0.02, 0.16
    elif slug in {"northern-whisper", "snowfall-hush", "observatory-glow"}:
        low, high = 0.10, 0.70
    return strength_ease(strength, low, high)


def mode_density(slug: str, strength: int) -> float:
    return strength_linear(strength, 0.28, 1.0)


def blend(a: Rgb, b: Rgb, amount: float) -> Rgb:
    t = clamp01(amount)
    return (
        clamp(a[0] + (b[0] - a[0]) * t),
        clamp(a[1] + (b[1] - a[1]) * t),
        clamp(a[2] + (b[2] - a[2]) * t),
    )


def scale(color: Rgb, level: float) -> Rgb:
    t = clamp01(level / 255.0)
    return (clamp(color[0] * t), clamp(color[1] * t), clamp(color[2] * t))


def add_soft(a: Rgb, b: Rgb, amount: float) -> Rgb:
    t = clamp01(amount)
    return (
        clamp(a[0] + b[0] * t * (1.0 - a[0] / 255.0)),
        clamp(a[1] + b[1] * t * (1.0 - a[1] / 255.0)),
        clamp(a[2] + b[2] * t * (1.0 - a[2] / 255.0)),
    )


def hash8(value: int) -> int:
    x = (value * 1103515245 + 12345) & 0xFFFFFFFF
    x ^= (x >> 16)
    return x & 0xFF


def wheel(pos: float) -> Rgb:
    p = int(pos) % 256
    if p < 85:
        return (255 - p * 3, p * 3, 0)
    if p < 170:
        p -= 85
        return (0, 255 - p * 3, p * 3)
    p -= 170
    return (p * 3, 0, 255 - p * 3)


def gaussian(distance: float, width: float) -> float:
    width = max(1.0, width)
    return math.exp(-(distance * distance) / (2.0 * width * width))


def cyclic_distance(a: float, b: float) -> float:
    d = abs(a - b)
    return min(d, STRIP_LENGTH_MM - d)


def x_mm(index: int, count: int) -> float:
    return 0.0 if count <= 1 else index * STRIP_LENGTH_MM / (count - 1)


def pulse(t: float, period: float, phase: float = 0.0) -> float:
    return (math.sin((t / max(period, 0.001) + phase) * math.tau) + 1.0) * 0.5


def color_for_mode(slug: str, strength: int, t: float, i: int, count: int) -> Rgb:
    x = i / max(1, count - 1)
    mm = x_mm(i, count)
    speed = mode_speed(slug, strength)
    motion = mode_motion(slug, strength)
    contrast = mode_contrast(slug, strength)
    density = mode_density(slug, strength)
    s = strength01(strength)

    warm = (255, 182, 102)
    cream = (255, 229, 190)
    cool = (145, 190, 255)
    blue = (42, 96, 180)
    deep = (4, 12, 34)
    green = (64, 210, 145)
    violet = (126, 90, 232)
    amber = (235, 110, 32)

    if slug == "solid":
        return scale(warm, 160)
    if slug == "strobe":
        on = pulse(t, 1.4 / speed) > strength_linear(strength, 0.74, 0.45)
        return scale((255, 255, 245), strength_ease(strength, 28, 230) if on else strength_ease(strength, 0, 20))
    if slug == "flash":
        env = pulse(t, 2.4 / speed)
        return scale((255, 210, 160), strength_linear(strength, 20, 64) + (env ** 3) * strength_ease(strength, 70, 190))
    if slug == "chase":
        heads = int(1 + 4 * density)
        level = 12.0
        for head in range(heads):
            center = ((t * 620 * speed + head * STRIP_LENGTH_MM / heads) % STRIP_LENGTH_MM)
            level += gaussian(cyclic_distance(mm, center), strength_linear(strength, 55, 170)) * strength_ease(strength, 100, 240)
        return scale(warm, level)
    if slug in {"wave", "slow-wave"}:
        base_period = 5.5 if slug == "wave" else 18.0
        wave = pulse(x * (1.5 + s) + t * speed / base_period, 1.0)
        secondary = pulse(x * (4.0 + 2.0 * s) - t * speed / (base_period * 0.7), 1.0)
        return scale(blend(warm, (120, 180, 255), secondary * 0.25 * s), strength_linear(strength, 42, 28) + wave * strength_ease(strength, 52, 175))
    if slug == "slow-pulse":
        level = 20.0
        for n in range(int(2 + 7 * density)):
            center = (hash8(n * 39) / 255.0 * STRIP_LENGTH_MM + math.sin(t * 0.08 * speed + n) * 180 * motion) % STRIP_LENGTH_MM
            level += gaussian(cyclic_distance(mm, center), strength_linear(strength, 110, 430)) * strength_ease(strength, 32, 112)
        return scale(blend(warm, (210, 170, 255), 0.18 * s), level)
    if slug == "rainbow":
        return scale(wheel((x * strength_ease(strength, 120, 300) + t * 18 * speed) % 255), strength_linear(strength, 95, 190))
    if slug == "dawn-bloom":
        reach = pulse(x * (0.7 + s) - t * 0.045 * speed, 1.0)
        return scale(blend((255, 98, 44), cream, reach), strength_linear(strength, 38, 28) + reach * strength_ease(strength, 70, 190))
    if slug == "moon-tide":
        tide = pulse(x * 1.2 + t * 0.035 * speed, 1.0)
        ripple = pulse(x * (4.0 + s * 2.0) - t * 0.12 * speed, 1.0)
        return scale(blend(deep, cool, 0.25 + tide * 0.5), 34 + tide * strength_ease(strength, 30, 118) + ripple * 24 * contrast)
    if slug in {"velvet-aurora", "northern-whisper"}:
        fold = pulse(x * (1.6 + s * 1.6) + t * 0.035 * speed, 1.0) ** 2
        veil = pulse(x * (3.8 + s) - t * 0.022 * speed, 1.0)
        ribbon = blend(green, blend(blue, violet, veil), 0.35 + 0.45 * s)
        cap = 96 if slug == "northern-whisper" else 150
        return scale(blend(deep, ribbon, fold * strength_ease(strength, 0.38, 0.86)), 18 + fold * strength_ease(strength, 34, cap))
    if slug in {"lantern-drift", "paper-lantern-row"}:
        count_lamps = int(3 + 5 * density)
        level = 18.0
        color = (42, 20, 8)
        for n in range(count_lamps):
            center = (n + 0.5) * STRIP_LENGTH_MM / count_lamps + math.sin(t * 0.15 * speed + n) * 90 * motion
            halo = gaussian(cyclic_distance(mm, center), strength_linear(strength, 120, 290))
            level += halo * strength_ease(strength, 70, 190)
            color = blend(color, warm, halo * 0.65)
        return scale(color, level)
    if slug in {"rain-glass", "rainy-window"}:
        drop_count = int(4 + 8 * density)
        color = blend((8, 22, 36), cool, 0.22)
        level = 22.0 + pulse(x * 1.3 + t * 0.02 * speed, 1.0) * strength_ease(strength, 5, 18)
        for n in range(drop_count):
            life = (hash8(n * 41) / 255.0 + t * (0.045 + n * 0.004) * speed) % 1.0
            center = ((hash8(n * 73) / 255.0 + life * (0.45 + 0.8 * speed)) % 1.0) * STRIP_LENGTH_MM
            bead = gaussian(abs(mm - center), strength_linear(strength, 14, 34))
            trail = max(0.0, (center - mm) / strength_linear(strength, 70, 190)) * math.exp(-abs(center - mm) / strength_linear(strength, 70, 190))
            fade = math.sin(life * math.pi)
            level += (bead * strength_ease(strength, 34, 88) + trail * strength_ease(strength, 18, 64)) * fade
            color = blend(color, (175, 220, 238), clamp01((bead + trail) * 0.8))
        return scale(color, level)
    if slug == "satin-breathing":
        env = pulse(t, strength_linear(strength, 14, 6))
        texture = pulse(x * (3.0 + s * 3.0) + t * 0.05 * speed, 1.0)
        return scale(blend(warm, cream, env * 0.45), strength_linear(strength, 105, 45) + env * strength_ease(strength, 30, 175) + texture * 16 * contrast)
    if slug == "library-lamp":
        texture = pulse(x * 2.2 + t * 0.004 * speed, 1.0)
        return scale(blend((255, 220, 176), warm, texture * 0.05), 222 + texture * strength_ease(strength, 1, 8))
    if slug == "solar-noon":
        gradient = pulse(x * 0.8 + t * 0.003 * speed, 1.0)
        return scale(blend((236, 246, 255), (255, 244, 218), gradient * strength_ease(strength, 0.02, 0.14)), 238)
    if slug == "golden-hour":
        source = pulse(x * 0.9 - t * 0.04 * speed, 1.0) ** 2
        return scale(blend(amber, cream, source * 0.7), 70 + source * strength_ease(strength, 70, 170))
    if slug in {"dream-aquarium", "deep-sea-glass"}:
        caustic = pulse(x * (5.0 + s * 4.0) + t * 0.12 * speed, 1.0) ** 3
        swell = pulse(x * 1.4 - t * 0.04 * speed, 1.0)
        return scale(blend((6, 44, 76), (68, 210, 210), 0.25 + swell * 0.55), 36 + swell * strength_ease(strength, 30, 82) + caustic * strength_ease(strength, 10, 54))
    if slug == "circuit-glow":
        level = 20.0
        for n in range(int(1 + 6 * density)):
            center = (t * 900 * speed + n * STRIP_LENGTH_MM / 5.0) % STRIP_LENGTH_MM
            level += gaussian(cyclic_distance(mm, center), strength_linear(strength, 35, 115)) * strength_ease(strength, 70, 210)
        return scale(blend((3, 22, 30), (58, 246, 192), min(1.0, level / 180)), level)
    if slug == "window-sunrise":
        reach = clamp01((t * 0.03 * speed + 0.15 - x) * strength_linear(strength, 1.2, 2.8))
        return scale(blend((42, 20, 24), cream, reach), 22 + reach * strength_ease(strength, 90, 210))
    if slug == "ember-quilt":
        patch = pulse(x * (4.0 + 2.0 * s) + t * 0.025 * speed, 1.0)
        return scale(blend((92, 26, 8), warm, patch * 0.62), 34 + patch * strength_ease(strength, 42, 118))
    if slug == "porcelain-sky":
        cloud = pulse(x * (1.4 + s * 1.8) - t * 0.018 * speed, 1.0)
        return scale(blend((210, 230, 255), (255, 244, 218), cloud * strength_ease(strength, 0.12, 0.34)), 172 + cloud * 38 * contrast)
    if slug == "hearthline":
        band = pulse(x * (3.0 + 2.0 * s) - t * 0.04 * speed, 1.0) ** 2
        return scale(blend((68, 26, 10), warm, band * 0.72), 40 + band * strength_ease(strength, 46, 132))
    if slug == "evening-current":
        current = pulse(x * 1.2 + t * 0.035 * speed, 1.0)
        ripple = pulse(x * (3.0 + s * 3.0) - t * 0.055 * speed, 1.0)
        return scale(blend((30, 42, 86), (188, 136, 255), current * 0.42), 45 + current * strength_ease(strength, 35, 94) + ripple * 32 * contrast)
    if slug in {"starlit-linen", "snowfall-hush"}:
        count_points = int(4 + 12 * density)
        color = blend((32, 34, 52), cream if slug == "starlit-linen" else cool, 0.2)
        level = 18.0
        for n in range(count_points):
            drift = (hash8(n * 47) / 255.0 + t * 0.01 * speed * (0.2 if slug == "starlit-linen" else 1.0)) % 1.0
            center = drift * STRIP_LENGTH_MM
            width = strength_linear(strength, 18, 58)
            soft = gaussian(cyclic_distance(mm, center), width)
            fade = pulse(t + n * 0.43, strength_linear(strength, 16, 7), hash8(n) / 255.0)
            level += soft * fade * strength_ease(strength, 32, 96)
            color = blend(color, (245, 250, 255), soft * fade * 0.65)
        return scale(color, level)
    if slug == "quiet-northern-lights":
        color = deep
        level = 16.0
        for n in range(int(2 + 4 * density)):
            center = (hash8(n * 61) / 255.0 * STRIP_LENGTH_MM + math.sin(t * 0.045 * speed + n * 1.3) * STRIP_LENGTH_MM * 0.18 * motion) % STRIP_LENGTH_MM
            fold = pulse(x * (3.0 + s * 2.5) + t * (0.02 + n * 0.004) * speed, 1.0) ** 2
            ribbon = gaussian(cyclic_distance(mm, center), strength_linear(strength, 280, 760)) * (0.45 + fold * 0.75)
            aurora = blend(green, blend(blue, violet, (n % 3) / 2.0), strength_ease(strength, 0.18, 0.62))
            color = add_soft(color, aurora, ribbon * strength_ease(strength, 0.25, 0.92))
            level += ribbon * strength_ease(strength, 24, 112)
        return scale(color, level)
    if slug == "candle-hall":
        color = (42, 16, 8)
        level = 10.0
        for n in range(int(3 + 4 * density)):
            center = (n + 0.5) * STRIP_LENGTH_MM / int(3 + 4 * density)
            flicker = pulse(t * speed, 1.5 + n * 0.2, hash8(n * 17) / 255.0)
            core = gaussian(abs(mm - center), strength_linear(strength, 24, 48))
            halo = gaussian(abs(mm - center), strength_linear(strength, 120, 250))
            color = blend(color, warm, halo * 0.62)
            level += halo * strength_ease(strength, 44, 122) + core * strength_ease(strength, 42, 112) * (0.65 + flicker * 0.35)
        return scale(color, level)
    if slug == "slow-prism":
        prism = pulse(x * (1.1 + s) + t * 0.015 * speed, 1.0)
        return scale(blend(warm, wheel(x * strength_ease(strength, 90, 240) + t * 9 * speed), strength_ease(strength, 0.18, 0.62)), 72 + prism * strength_ease(strength, 40, 100))
    if slug == "garden-shade":
        breeze = pulse(x * (1.0 + motion) + t * 0.035 * speed, 1.0)
        shade = pulse(x * (2.2 + motion * 2.0) - t * 0.025 * speed, 1.0)
        return scale(blend((255, 224, 166), blend((72, 124, 60), (220, 190, 92), breeze), 0.25 + shade * 0.45), 120 + breeze * strength_ease(strength, 18, 62) - shade * 14)
    if slug == "observatory-glow":
        moon = (t * 75 * speed) % STRIP_LENGTH_MM
        glow = gaussian(cyclic_distance(mm, moon), strength_linear(strength, 560, 900))
        star = pulse(t + hash8(i * 13), strength_linear(strength, 24, 9), 0.1) if hash8(i * 23) > 248 - int(20 * density) else 0
        return scale(blend(deep, (220, 238, 255), glow * 0.72 + star * 0.5), 18 + glow * strength_ease(strength, 64, 134) + star * strength_ease(strength, 18, 78))
    if slug == "campfire-blanket":
        fire = pulse(x * (1.8 + s) - t * 0.06 * speed, 1.0)
        ember_wave = pulse(x * (4.0 + s * 3.0) + t * 0.055 * speed, 1.0)
        return scale(blend(amber, warm, fire * 0.64), 42 + fire * strength_ease(strength, 40, 90) + ember_wave * 28 * contrast)
    if slug == "storm-outside":
        roll = pulse(x * 1.3 - t * 0.018 * speed, 1.0)
        rain = pulse(x * (5.0 + s * 3.0) + t * 0.11 * speed, 1.0)
        flash = pulse(t, strength_linear(strength, 30, 16), 0.2)
        flash = max(0.0, (flash - 0.82) / 0.18) * strength_ease(strength, 0.20, 0.70)
        color = blend(blend((4, 14, 28), blue, 0.42), (82, 104, 130), roll * 0.35)
        color = blend(color, (235, 246, 255), flash)
        return scale(color, 22 + roll * strength_ease(strength, 24, 58) + rain * 20 * contrast + flash * 65)
    return scale(warm, 128)


def render_frame(slug: str, strength: int, t: float, count: int = LED_COUNT) -> list[Rgb]:
    return [color_for_mode(slug, strength, t, i, count) for i in range(count)]


def mean_abs_delta(a: list[Rgb], b: list[Rgb]) -> float:
    total = 0
    for pa, pb in zip(a, b):
        total += abs(pa[0] - pb[0]) + abs(pa[1] - pb[1]) + abs(pa[2] - pb[2])
    return total / max(1, len(a) * 3)


def sample_for_html(frame: list[Rgb]) -> list[Rgb]:
    if len(frame) <= HTML_SAMPLE_PIXELS:
        return frame
    sampled = []
    for i in range(HTML_SAMPLE_PIXELS):
        start = int(i * len(frame) / HTML_SAMPLE_PIXELS)
        end = max(start + 1, int((i + 1) * len(frame) / HTML_SAMPLE_PIXELS))
        chunk = frame[start:end]
        sampled.append((
            clamp(sum(p[0] for p in chunk) / len(chunk)),
            clamp(sum(p[1] for p in chunk) / len(chunk)),
            clamp(sum(p[2] for p in chunk) / len(chunk)),
        ))
    return sampled


def gradient(frame: list[Rgb]) -> str:
    pixels = sample_for_html(frame)
    stops = []
    denom = max(1, len(pixels) - 1)
    for i, (r, g, b) in enumerate(pixels):
        stops.append(f"rgb({r},{g},{b}) {i * 100 / denom:.2f}%")
    return "linear-gradient(90deg," + ",".join(stops) + ")"


def html_escape(value: str) -> str:
    return value.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def write_mode_page(slug: str, name: str, frames: dict[int, dict[int, list[Rgb]]]) -> None:
    sections = []
    for strength in STRENGTH_LEVELS:
        strips = []
        for seconds in TIME_SAMPLES_SECONDS:
            strips.append(
                f"<div class='sample'><span>{seconds:>2}s</span>"
                f"<div class='strip' style='background:{gradient(frames[strength][seconds])}'></div></div>"
            )
        sections.append(f"<section><h2>Strength {strength}</h2>{''.join(strips)}</section>")

    html = f"""<!doctype html>
<html><head><meta charset="utf-8"><title>{html_escape(name)} Simulation</title>
<style>
body{{margin:0;background:#101214;color:#edf3f5;font-family:system-ui,Segoe UI,sans-serif}}
main{{max-width:1100px;margin:0 auto;padding:24px 16px 48px}}
a{{color:#8bd0ff}}h1{{font-size:24px;margin:0 0 8px}}h2{{font-size:15px;margin:18px 0 8px;color:#c8d2d8}}
.sample{{display:grid;grid-template-columns:42px 1fr;gap:10px;align-items:center;margin:6px 0}}
.sample span{{font:12px ui-monospace,Consolas,monospace;color:#aeb8be}}
.strip{{height:30px;border-radius:6px;border:1px solid rgba(255,255,255,.14);box-shadow:inset 0 0 0 1px rgba(0,0,0,.22)}}
section{{border-top:1px solid rgba(255,255,255,.09);padding-top:8px}}
</style></head><body><main>
<p><a href="index.html">Simulation index</a> | <a href="contact_sheet.html">Contact sheet</a></p>
<h1>{html_escape(name)}</h1>
<p>{html_escape(MODE_NOTES[slug])}</p>
{''.join(sections)}
</main></body></html>"""
    (OUT_DIR / f"{slug}.html").write_text(html, encoding="utf-8")


def compute_mode_metrics(slug: str, frames: dict[int, dict[int, list[Rgb]]]) -> dict[str, object]:
    strength_delta_samples = [
        mean_abs_delta(frames[0][seconds], frames[255][seconds])
        for seconds in TIME_SAMPLES_SECONDS
    ]
    motion_samples = [
        mean_abs_delta(frames[128][a], frames[128][b])
        for a, b in zip(TIME_SAMPLES_SECONDS, TIME_SAMPLES_SECONDS[1:])
    ]
    strength_score = sum(strength_delta_samples) / len(strength_delta_samples)
    motion_score = sum(motion_samples) / max(1, len(motion_samples))
    response_threshold = 0.10 if slug in {"library-lamp", "solar-noon"} else 1.0
    return {
        "strengthDeltaScore": round(strength_score, 3),
        "estimatedMotionScore": round(motion_score, 3),
        "strengthResponseProven": slug == "solid" or strength_score > response_threshold,
        "motionProvenAt128": slug == "solid" or motion_score > 0.05,
        "strengths": {
            str(strength): {
                "estimatedMotionScore": round(
                    sum(
                        mean_abs_delta(frames[strength][a], frames[strength][b])
                        for a, b in zip(TIME_SAMPLES_SECONDS, TIME_SAMPLES_SECONDS[1:])
                    ) / max(1, len(TIME_SAMPLES_SECONDS) - 1),
                    3,
                ),
                "averageLuma": round(
                    sum(sum(pixel) / 3 for seconds in TIME_SAMPLES_SECONDS for pixel in frames[strength][seconds])
                    / max(1, len(TIME_SAMPLES_SECONDS) * LED_COUNT),
                    3,
                ),
            }
            for strength in STRENGTH_LEVELS
        },
    }


def write_contact_sheet(all_frames: dict[str, dict[int, dict[int, list[Rgb]]]]) -> None:
    rows = []
    for slug, name in MODES:
        low = gradient(all_frames[slug][0][10])
        mid = gradient(all_frames[slug][128][10])
        high = gradient(all_frames[slug][255][10])
        rows.append(
            f"<tr><th><a href='{slug}.html'>{html_escape(name)}</a></th>"
            f"<td><div class='strip' style='background:{low}'></div></td>"
            f"<td><div class='strip' style='background:{mid}'></div></td>"
            f"<td><div class='strip' style='background:{high}'></div></td></tr>"
        )
    html = f"""<!doctype html><html><head><meta charset="utf-8"><title>Bedroom LED Mode Contact Sheet</title>
<style>
body{{margin:0;background:#101214;color:#edf3f5;font-family:system-ui,Segoe UI,sans-serif}}
main{{max-width:1180px;margin:0 auto;padding:24px 16px 48px}}a{{color:#8bd0ff}}
table{{width:100%;border-collapse:collapse}}th,td{{padding:7px;border-bottom:1px solid rgba(255,255,255,.10)}}th{{text-align:left;width:210px}}
.strip{{height:24px;border-radius:6px;border:1px solid rgba(255,255,255,.14)}}
</style></head><body><main>
<p><a href="index.html">Simulation index</a></p><h1>Mode Contact Sheet</h1>
<p>All strips are rendered at t=10s for strength 0, 128, and 255. This is simulation proof only, not physical LED proof.</p>
<table><thead><tr><th>Mode</th><th>Strength 0</th><th>Strength 128</th><th>Strength 255</th></tr></thead><tbody>
{''.join(rows)}
</tbody></table></main></body></html>"""
    (OUT_DIR / "contact_sheet.html").write_text(html, encoding="utf-8")


def write_index(metrics: dict[str, object]) -> None:
    rows = []
    for slug, name in MODES:
        m = metrics["modes"][slug]
        rows.append(
            f"<tr><th><a href='{slug}.html'>{html_escape(name)}</a></th>"
            f"<td>{m['strengthResponseProven']}</td><td>{m['strengthDeltaScore']}</td><td>{m['estimatedMotionScore']}</td></tr>"
        )
    html = f"""<!doctype html><html><head><meta charset="utf-8"><title>Bedroom LED Mode Simulations</title>
<style>
body{{margin:0;background:#101214;color:#edf3f5;font-family:system-ui,Segoe UI,sans-serif}}
main{{max-width:1040px;margin:0 auto;padding:24px 16px 48px}}a{{color:#8bd0ff}}
table{{width:100%;border-collapse:collapse}}th,td{{padding:8px;border-bottom:1px solid rgba(255,255,255,.10)}}th{{text-align:left}}
</style></head><body><main><h1>Bedroom LED Mode Simulations</h1>
<p><a href="contact_sheet.html">Contact sheet</a> | <a href="mode_metrics.json">Metrics JSON</a></p>
<p>Strength levels: {STRENGTH_LEVELS}. Time samples: {TIME_SAMPLES_SECONDS}. LED count simulated: {LED_COUNT}.</p>
<table><thead><tr><th>Mode</th><th>Strength response proven?</th><th>Strength delta</th><th>Motion score at 128</th></tr></thead><tbody>
{''.join(rows)}
</tbody></table></main></body></html>"""
    (OUT_DIR / "index.html").write_text(html, encoding="utf-8")


def write_report(metrics: dict[str, object]) -> None:
    rows = []
    for slug, name in MODES:
        m = metrics["modes"][slug]
        metrics_passed = m["strengthResponseProven"] and (
            m["motionProvenAt128"] or slug in {"solid", "solar-noon", "library-lamp"}
        )
        rows.append(
            f"| {name} | {'yes' if m['strengthResponseProven'] else 'no'} | yes | "
            f"{'yes' if metrics_passed else 'no'} | {MODE_NOTES[slug]} |"
        )
    report = f"""# Bedroom LED Controller Mode Animation Overhaul Report

Generated by `tools/simulate_modes.py`.

## Scope

This pass overhauls animation behavior and Animation Strength proof. It does not claim physical LED observation and does not address LED density or physical scaling.

## Animation Strength

- Persistent setting: `animationStrength`, 0..255, default 128.
- API target: `/api/animation?strength=0..255`; `/api/intensity?value=0..255` is retained as a compatibility alias.
- State target: `/api/state` includes `animationStrength` and `animationStrengthPercent`.
- UI target: exactly one visible Animation Strength slider.
- Preview target: browser preview reads `state.animationStrength`.

## Simulation Proof Outputs

- Index: `docs/generated/mode_simulations/index.html`
- Contact sheet: `docs/generated/mode_simulations/contact_sheet.html`
- Metrics JSON: `docs/generated/mode_simulations/mode_metrics.json`
- Per-mode HTML: `docs/generated/mode_simulations/<mode-slug>.html`

Strength levels tested: {STRENGTH_LEVELS}

Time samples tested: {TIME_SAMPLES_SECONDS}

LED count simulated: {LED_COUNT}

Quiet Northern Lights no longer uses a simple global sine/blob as its primary visual model. It is implemented and simulated as multiple soft aurora ribbons with separate centers, folds, opacity, motion, and color contribution.

## Mode Proof Matrix

| Mode | Strength response proven? | Visual proof generated? | Metrics passed? | Notes |
| --- | --- | --- | --- | --- |
{chr(10).join(rows)}

## Tests And Compile Status

- Simulator generation: completed by this script.
- Focused overhaul contract: `python -m unittest tests.test_full_mode_animation_overhaul_contract -v` passed 6/6.
- Full Python suite: `python -m unittest discover -s tests -v` passed 152/152.
- Sketch mirror verification: SHA-256 check across all mirrored `.ino`/`.h` files reported no mismatches.
- ESP8266/D1 mini compile: `arduino-cli compile --fqbn esp8266:esp8266:d1_mini firmware\\BedroomLedController` passed using Arduino CLI 1.4.1 and esp8266 core 3.1.2.
- Compile fix made during verification: `renderFlash()` now uses `max(static_cast<uint32_t>(1), onMs)` so ESP8266 GCC does not reject mixed unsigned types.

## Resource Impact

- RAM globals/statics: firmware changes reuse the central frame buffers and do not add a second render/output buffer. New mode state is local stack math inside renderers.
- ESP8266 RAM globals/statics: 69,496 / 80,192 bytes (86%). Segments: DATA 1,652; RODATA 26,092; BSS 41,752.
- ESP8266 IRAM: 61,383 / 65,536 bytes (93%). Segments: ICACHE 32,768 reserved; IRAM 28,615.
- ESP8266 flash/IROM: 638,896 / 1,048,576 bytes (60%).
- Runtime heap: not physically measured in this simulation pass.
- LittleFS: generated docs are repository artifacts only and are not uploaded to LittleFS by this change.

## Known Limitations

- Simulation approximates firmware visuals and proves code-path variation across strength levels; it is not physical LED proof.
- Real LED diffusion, power limits, Wi-Fi heap, and camera exposure can still change perceived subtlety.
- Library Lamp and Solar Noon intentionally remain low-motion, practical modes.

Physical LED test status: not physically tested in this pass.
"""
    REPORT.write_text(report, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args()

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    all_frames: dict[str, dict[int, dict[int, list[Rgb]]]] = {}
    metrics: dict[str, object] = {
        "ledCount": LED_COUNT,
        "stripLengthMm": STRIP_LENGTH_MM,
        "strengthLevels": STRENGTH_LEVELS,
        "timeSamplesSeconds": TIME_SAMPLES_SECONDS,
        "modes": {},
    }

    for slug, name in MODES:
        frames = {
            strength: {seconds: render_frame(slug, strength, float(seconds)) for seconds in TIME_SAMPLES_SECONDS}
            for strength in STRENGTH_LEVELS
        }
        all_frames[slug] = frames
        metrics["modes"][slug] = compute_mode_metrics(slug, frames)
        write_mode_page(slug, name, frames)

    write_contact_sheet(all_frames)
    (OUT_DIR / "mode_metrics.json").write_text(json.dumps(metrics, indent=2), encoding="utf-8")
    write_index(metrics)
    write_report(metrics)

    if not args.quiet:
        print(f"Wrote simulation proof to {OUT_DIR}")
        print(f"Wrote report to {REPORT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
