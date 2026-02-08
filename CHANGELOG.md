# Changelog

All notable changes to this project will be documented in this file.

## [1.0.1] - 2026-02-08

### Fixed
- **Victory Condition**: Relaxed the win check to accept any board state that visually matches the row and column clues. This fixes an issue where valid solutions were rejected because they didn't match the internal hidden tile structure.

## [1.0.0] - 2026-02-08

### Changed
- **Rebrand**: Project renamed from Kuro to Tilepaint to reflect its distinct mechanics.
- **Toolkit**: Ported the entire application to **GTK4** and **Libadwaita**.
- **UI**: Replaced `.ui` files with **Blueprint** (`.blp`) for modern UI definition.

### Added
- **Unique Games**: Integrated a backtracking solver to ensure every board has exactly one unique solution.
- **Visuals**: Consistent board rendering (uniform grid) and stylized clue numbers preventing solution leaks.
- **Pause Feature**: Added a new pause overlay that hides clue numbers to prevent peeking.
- **Keyboard Support**: Full keyboard navigation and gameplay support (Arrows, WASD, HJKL).
- **Board Themes**: Added "Auto", "Kuro" (Dark), and "Hitori" (Light) board themes.
- **Preferences**: New preferences UI integrated into the primary menu with radio-style selectors.
- **Headerbar Subtitle**: Added "Logic Puzzle Game" subtitle to the main window.
- **Project Identity**: Updated branding, icons, and metadata to "Tilepaint".

### Acknowledgements
This release builds upon the excellent work of the original Hitori developers, Philip Withnall and Ben Windsor. Special thanks to the GTK and Libadwaita teams for providing the modern toolkit that makes Kuro possible.
