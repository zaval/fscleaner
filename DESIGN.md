---
name: Technical Precision Utility
colors:
  surface: '#faf9fe'
  surface-dim: '#dad9df'
  surface-bright: '#faf9fe'
  surface-container-lowest: '#ffffff'
  surface-container-low: '#f4f3f8'
  surface-container: '#eeedf3'
  surface-container-high: '#e9e7ed'
  surface-container-highest: '#e3e2e7'
  on-surface: '#1a1b1f'
  on-surface-variant: '#414755'
  inverse-surface: '#2f3034'
  inverse-on-surface: '#f1f0f5'
  outline: '#717786'
  outline-variant: '#c1c6d7'
  surface-tint: '#005bc1'
  primary: '#0058bc'
  on-primary: '#ffffff'
  primary-container: '#0070eb'
  on-primary-container: '#fefcff'
  inverse-primary: '#adc6ff'
  secondary: '#4c4aca'
  on-secondary: '#ffffff'
  secondary-container: '#6664e4'
  on-secondary-container: '#fffbff'
  tertiary: '#894d00'
  on-tertiary: '#ffffff'
  tertiary-container: '#ac6300'
  on-tertiary-container: '#fffbff'
  error: '#ba1a1a'
  on-error: '#ffffff'
  error-container: '#ffdad6'
  on-error-container: '#93000a'
  primary-fixed: '#d8e2ff'
  primary-fixed-dim: '#adc6ff'
  on-primary-fixed: '#001a41'
  on-primary-fixed-variant: '#004493'
  secondary-fixed: '#e2dfff'
  secondary-fixed-dim: '#c2c1ff'
  on-secondary-fixed: '#0c006a'
  on-secondary-fixed-variant: '#3631b4'
  tertiary-fixed: '#ffdcbf'
  tertiary-fixed-dim: '#ffb874'
  on-tertiary-fixed: '#2d1600'
  on-tertiary-fixed-variant: '#6a3b00'
  background: '#faf9fe'
  on-background: '#1a1b1f'
  surface-variant: '#e3e2e7'
typography:
  display-lg:
    fontFamily: Inter
    fontSize: 28px
    fontWeight: '600'
    lineHeight: 34px
    letterSpacing: -0.02em
  headline-md:
    fontFamily: Inter
    fontSize: 20px
    fontWeight: '600'
    lineHeight: 26px
    letterSpacing: -0.01em
  title-sm:
    fontFamily: Inter
    fontSize: 15px
    fontWeight: '600'
    lineHeight: 20px
  body-md:
    fontFamily: Inter
    fontSize: 14px
    fontWeight: '400'
    lineHeight: 20px
  body-sm:
    fontFamily: Inter
    fontSize: 12px
    fontWeight: '400'
    lineHeight: 16px
  label-caps:
    fontFamily: Inter
    fontSize: 11px
    fontWeight: '700'
    lineHeight: 16px
    letterSpacing: 0.05em
  mono-data:
    fontFamily: jetbrainsMono
    fontSize: 12px
    fontWeight: '400'
    lineHeight: 16px
rounded:
  sm: 0.25rem
  DEFAULT: 0.5rem
  md: 0.75rem
  lg: 1rem
  xl: 1.5rem
  full: 9999px
spacing:
  unit: 4px
  container-padding: 24px
  element-gap: 12px
  list-item-padding: 10px 16px
  grid-gutter: 16px
---

## Brand & Style

This design system is built for a high-performance desktop utility where trust and clarity are paramount. The brand personality is **methodical, transparent, and authoritative**. It seeks to evoke a sense of "digital hygiene"—the feeling of a well-oiled machine being restored to its optimal state.

The visual style leverages **Corporate Minimalism** with a focus on data density and clear actionable states. It avoids unnecessary ornamentation in favor of high-legibility typography and functional depth. While inspired by macOS native aesthetics, it introduces a more structured, systematic approach to information display, ensuring that complex file paths and system data remain scannable. The emotional response should be one of confidence: the user is in total control of their system's health.

## Colors

The palette is centered around **Tech Blue (#007AFF)**, utilized strictly for primary actions, active selection states, and indicating progress. This is balanced by a sophisticated range of neutral grays that define the "Apple-tier" high-end utility feel.

- **Primary:** Used for the main CTA (e.g., "Scan Now"), toggle switches, and selected list item backgrounds.
- **Surface Strategy:** We use a tiered background approach. The main window background is a cool, light gray (`#F5F5F7`), while interactive cards and content containers use pure white (`#FFFFFF`) to create a clear "raised" visual priority.
- **Semantic Accents:** Tertiary Orange is reserved for warning states (e.g., large cache files), while a standard success green should be used only for "System Clean" confirmations.

## Typography

The typography system relies on **Inter** for its exceptional legibility at small sizes—critical for viewing long file paths and technical metadata.

- **Hierarchy:** We use weight (SemiBold vs Regular) rather than drastic size changes to differentiate levels. This maintains a compact, utility-first layout.
- **Technical Content:** For file paths, directories, and memory sizes (e.g., `/Users/name/Library/Caches`), use **JetBrains Mono**. This monospaced font provides a distinct "technical" feel and ensures that characters like '0' and 'O' or 'l' and '1' are easily distinguishable during file selection.
- **Labels:** Small caps are used for section headers in sidebars to provide structural rigidity without adding visual bulk.

## Layout & Spacing

This design system follows a **Fixed-Fluid Hybrid** model optimized for desktop windows.

- **Sidebar/Navigation:** Fixed width (240px–280px) to allow for consistent tool switching.
- **Main Content:** Fluid width with a maximum readable container of 960px.
- **Rhythm:** A 4px baseline grid ensures tight, professional alignment. Most internal spacing between grouped elements (like a folder icon and its label) should be 12px (3 units), while major sections are separated by 24px or 32px.
- **Safe Zones:** Content should never bleed to the edge of the window; a minimum 24px margin is required on all sides to maintain the "clean" aesthetic.

## Elevation & Depth

To achieve the "high-end utility" look, this design system uses **Tonal Layers** combined with **Ambient Shadows**.

1. **Level 0 (Base):** The main application background. Flat, neutral gray.
2. **Level 1 (Cards/Containers):** Pure white background with a 1px inner stroke (`rgba(0,0,0,0.05)`) and a very soft, diffused shadow (0px 4px 12px rgba(0,0,0,0.04)).
3. **Level 2 (Active/Hover):** When an item is selected or hovered, the shadow deepens slightly, and the Tech Blue accent is applied to the border or background.
4. **Modals/Popovers:** These use a higher elevation with a 20% backdrop blur (glassmorphism) behind them to keep the user focused on the immediate task while maintaining context of the file system behind.

## Shapes

The shape language is **Structured & Friendly**.

- **Standard Radius:** 8px for primary interactive elements like buttons, input fields, and list selection highlights.
- **Container Radius:** 12px (Large) for cards and main content areas to create a soft, modern frame for the data.
- **Consistency:** All icons must be contained within rounded-square enclosures or follow a consistent circular metaphor to match the UI's geometry. Avoid sharp corners entirely to distinguish the app from legacy Windows utilities.

## Components

- **Action Buttons:** Primary buttons are solid Tech Blue with white text. Secondary buttons use a white background with a subtle gray border.
- **Folder Cards:** Featured on the landing screen, these cards use a 12px radius, a 1px subtle border, and horizontal layouts. Icons are placed on the left, followed by the Folder Name (Title-sm) and Path (Mono-data).
- **Control Toggles:** Use the iOS-style pill toggle. Tech Blue for the 'On' state, light gray for 'Off'.
- **System Lists:** High-density rows (40px height) with a 1px divider. Checkboxes are placed on the far left, and "Trash" or "Reveal in Finder" actions appear on hover on the far right.
- **Progress Bars:** Thin (4px), using a Tech Blue fill on a light gray track. For destructive processes, the bar may pulse or change to a subtle indigo.
- **Input Fields:** Search bars and path inputs should have a background color slightly darker than the surface they sit on (e.g., `#F2F2F7`) to clearly indicate "editable" status.