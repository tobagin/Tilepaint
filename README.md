# Tilepaint

A modern, native GTK4 implementation of the Tilepaint logic puzzle game for GNOME.

<div align="center">

<img src="data/screenshots/main-window.png" width="800" alt="Main Window" />

<a href="https://flathub.org/apps/io.github.tobagin.Kuro"><img src="https://flathub.org/api/badge/io.github.tobagin.Kuro" height="52" alt="Get it on Flathub"></a>

</div>



## 🎉 Version 1.0.0 - Reimagined for GNOME

**Tilepaint 1.0.0** is a major release that modernizes the classic logic puzzle with a fresh new look and feel.

### ✨ Key Features

- **🚀 Native GTK4 & Libadwaita**: Built with the latest GNOME technologies for a seamless, modern experience.
- **🧩 Unique Puzzles**: Every game is guaranteed to have exactly one unique solution.
- **⏸️ Anti-Peeking Pause**: A pause screen that hides clue numbers to prevent cheating.
- **⌨️ Keyboard Support**: Play entirely with your keyboard using arrow keys, WASD, or HJKL.
- **🎨 Auto Theme**: Automatically adapts to your system's light or dark mode preference.
- **📏 Multiple Board Sizes**: Choose from 5x5 up to 10x10 grids to match your skill level.
- **UNDO/REDO**: Make mistakes without worry with full undo/redo support.

### 🆕 What's New in 1.0.1

- **Victory Condition Relaxed**: The game now accepts any valid visual solution that matches the row and column clues, resolving issues with "double solution" puzzles.

For detailed release notes and version history, see [CHANGELOG.md](CHANGELOG.md).

## Building from Source

```bash
# Clone the repository
git clone https://github.com/tobagin/Tilepaint.git
cd Tilepaint

# Build and install development version
./build.sh --dev
```

## Usage

### Basic Usage

Launch Tilepaint from your applications menu or run:
```bash
flatpak run io.github.tobagin.Tilepaint.Devel
```

1.  **Rule 1**: No number can appear more than once in a row or column.
2.  **Rule 2**: Painted cells cannot touch horizontally or vertically.
3.  **Rule 3**: All unpainted cells must be connected in a single group.

### Keyboard Controls

- **Arrow Keys / WASD / HJKL**: Move cursor.
- **Enter / Space**: Toggle cell state (Unpainted -> Painted -> Tag 1 -> Tag 2).
- **Ctrl+Z**: Undo.
- **Ctrl+Shift+Z**: Redo.
- **Ctrl+P**: Pause/Resume.
- **Ctrl+N**: New Game.
- **Ctrl+Q**: Quit.

## Privacy & Security

Tilepaint is designed to respect your privacy:

- **Sandboxed**: Distributed as a Flatpak with strict permissions.
- **Local Data**: All game progress and configuration is stored locally.
- **Open Source**: Code is fully available for audit.

## Contributing

Contributions are welcome! Please feel free to open issues or submit pull requests.

- Reporting Bugs: [GitHub Issues](https://github.com/tobagin/Tilepaint/issues)

## License

Tilepaint is licensed under the [GPL-3.0-or-later](COPYING).

## Acknowledgments

- **Original Developers**: Philip Withnall, Ben Windsor.
- **GNOME**: For the GTK toolkit and Libadwaita.
- **Nikoli**: For the original puzzle inspiration.

## Screenshots

| Dark Theme | Logic Gameplay |
|------------|----------------|
| ![Dark Theme](data/screenshots/dark-board-theme.png) | ![Logic Gameplay](data/screenshots/painted-squares.png) |

| Victory! | Pause Overlay |
|----------|---------------|
| ![Victory!](data/screenshots/you-win.png) | ![Pause Overlay](data/screenshots/pause.png) |

| High Scores | About Dialog |
|-------------|--------------|
| ![High Scores](data/screenshots/high-scores.png) | ![About Dialog](data/screenshots/about.png) |

| Keyboard Shortcuts |
|--------------------|
| ![Keyboard Shortcuts](data/screenshots/keyboard-shortcus.png) |
