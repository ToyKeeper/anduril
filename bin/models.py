#!/usr/bin/env python3

import os


def main():
    """Scan build targets and generate the MODELS output."""
    models = []

    # Load all cfg-*.h files
    for root, dirnames, _ in os.walk("hw"):
        for dirname in dirnames:
            # Skip top-level brand directories
            if "/" not in root:
                continue

            model = load_model(os.path.join(root, dirname))
            if model.mcu and model.model:
                models.append(model)

    # Sort by model number
    models.sort(key=lambda m: (m.model, m.name))

    col_sizes = [
        max(len(m.model) for m in models),
        max(len(m.mcu) for m in models),
        max(len(m.name) for m in models),
    ]

    print(f"Models: {len(models)}\n")

    fmt = f"%-{col_sizes[0]}s  %-{col_sizes[1]}s  %s"
    print(fmt % ("Model", "MCU", "Name"))
    print(fmt % ("-----", "---", "----"))

    for model in models:
        print(fmt % (model.model, model.mcu, model.name))

    print("\nDuplicates:")
    for index, model in enumerate(models):
        for other in models[index + 1:]:
            if model.model == other.model:
                print(f"{model.model}\t{model.name}, {other.name}")

    print("\nMissing:")
    for model in models:
        if not model.model:
            print(model.name)


class Model:
    """Container for model metadata."""

    def __init__(self):
        self.name = ""
        self.model = ""
        self.mcu = ""


def load_model(path):
    """Load model metadata from the filesystem."""
    model = Model()

    model.name = path.replace("hw/", "").replace("/", "-")
    model.mcu = inherit(path, "arch")
    model.model = inherit(path, "model")

    if model.model:
        model.model = model_translate(model.model)

    return model


def inherit(path, field):
    """Recursively inherit a field value from parent directories."""
    check_path = os.path.join(path, field)

    if os.path.exists(check_path):
        with open(check_path, encoding="utf-8") as file:
            return file.readline().strip()

    parent = os.path.dirname(path)
    if parent and parent != path:
        return inherit(parent, field)

    return None


def model_translate(value):
    """Convert raw ordinal hex codes into human-friendly a–f digits."""
    value = str(value)

    replacements = {
        chr(ord("0") + 10): "a",
        chr(ord("0") + 11): "b",
        chr(ord("0") + 12): "c",
        chr(ord("0") + 13): "d",
        chr(ord("0") + 14): "e",
        chr(ord("0") + 15): "f",
    }

    for source, replacement in replacements.items():
        value = value.replace(source, replacement)

    return value


if __name__ == "__main__":
    main()
