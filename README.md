# GBSPTools - Version 0.8
The GBSP, GVIS and GLIGHT tools together build and light your map for the Genesis3D Engine.

## Commands
### Shared by all tools
	// The .map (BSP) or .bsp (VIS and Light) file to process.
	mapname

	// Outputs detailed compilation progress information.
	// Default: Off
	-verbose

### BSP - Creates level geometry from .map
    // Outputs detailed entity information to the console window.
    // Default: Off
    -entverbose

    // Update entities from BSP.
    // Default: Off
    -updateents

### VIS - Performs potential visible set calculations on compiled level.

	// Performs full visibility calculations. When off, the calculated visibility is less efficient at run-time, but takes much less time to compile.
	// Default: Off
	-full

	// Sort the portals with MightSee.
	// Default: Off
	-sortportals

### Light - Performs calculations to add lighting effects to the level.
	// Outputs detailed compilation progress information to the console window.
	// Default: Off
	-verbose

	// Illuminates all surfaces with the light color specified.
	// Default: 0 0 0 | Range: 0-255 0-255 0-255
	-minlight r g b 

	// Light intensity multiplier for the entire level. Higher numbers make the level brighter, lower numbers make the level darker.
	// Default: 1.0
	-lightscale #

    // Face reflectivity multiplier. Higher numbers make the level brighter and more colorful.
    // Default: 1.0
    -reflectscale

	// Uses more samples to give finer lighting effects.
    // Default: Off
    -extra

	// Performs radiosity lighting of the level after the default direct lighting has been computed. Significantly increases compilation time.
	// Default: Off
	-radiosity

    // Number of times to bounce a ray during radiosity lighting. Higher numbers give more subtle lighting effects at the expense of compile time.
    // Default: 10
    -bounce #

    // Determines the size of the grid used when performing radiosity lighting. A larger patch size will produce less detailed and less sharp lighting effects. Smaller numbers increase light detail but also increase compilation time and memory required to compile the level.
    // Default: 128
    -patchsize #

    // Set fast patching for fast compiles.
    // Default: Off
    -fastpatch
