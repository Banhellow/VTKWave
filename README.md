# VTKWave

A desktop volume viewer built with **VTK** and **Qt**. It renders a CT scan and a co-registered MRI scan together as 3-D volumes. A corner of both volumes can be cut away to look inside, and the CT opacity threshold can be adjusted live.

The repository also contains an unfinished experiment that turns a height-map image into a voxel terrain you can scroll with the arrow keys (`VoxelMesh`, `PlaneMesh`, `ArrowInteractorStyle`, plus a skybox loader). This code is compiled, but it is not shown in the current window.

## Features

- CPU ray-cast volume rendering (`vtkFixedPointVolumeRayCastMapper`) of two 512 × 512 × 247 16-bit volumes:
  - CT, mapped grey-scale over 0–5257
  - MRI, mapped over 0–597
- **Slice X / Slice Y** sliders cut away the region where both `x < X` and `y < Y`, in both volumes at once. The voxel update is parallelised with OpenMP.
- **CT Density** slider moves the lower end of the CT opacity ramp, which hides low-density tissue.
- Standard VTK mouse interaction in the 3-D view: rotate, pan, zoom.

## Requirements

- CMake ≥ 3.12 and a C++ compiler with OpenMP support (developed with MSVC on Windows)
- **Qt 6** (Widgets, Core). The checked-in moc output was generated with Qt 6.6.2.
- **VTK** built with Qt support (the `GUISupportQt` module, which provides `QVTKOpenGLNativeWidget`) and the volume-rendering modules

## Input data (not included)

The program reads two raw volumes from the `images/` folder. **They are not part of the repository:**

| File | Contents |
|---|---|
| `images/CT.rdata` | CT volume |
| `images/MR_TO_CT.rdata` | MRI volume registered to the CT |

Each file must be raw, headerless `uint16` data (machine byte order) with these properties:

- 512 × 512 × 247 voxels, x fastest
- 129,499,136 bytes
- spacing 0.447266 × 0.447266 × 0.625 mm

A size mismatch is reported on the console.

## Building

```bash
git clone https://github.com/Banhellow/VTKWave.git
cd VTKWave
# put CT.rdata and MR_TO_CT.rdata into images/ first
cmake -S . -B build -DVTK_DIR=<path-to-VTK-build-or-install>/lib/cmake/vtk -DCMAKE_PREFIX_PATH=<path-to-Qt6>
cmake --build build --config Release
```

The build copies the `images/` folder next to the executable (a pre-link step).

## Running

The data paths (`images\...`) are relative to the current working directory, so start the program from the `build` folder, which contains the copied `images/` folder:

```bash
cd build
./VTKWave            # Visual Studio generator: Release\VTKWave.exe
```

On Windows, also make sure the Qt and VTK DLLs are on your `PATH` or copied next to the executable (for example with `windeployqt`).

## Project structure

```
CMakeLists.txt
headers/                 Class declarations (plus a stale moc_mainwindow.cpp that is not compiled)
src/
  main.cpp               Creates the Qt application and main window
  MainWindow.cpp         Qt layout: VTK widget + slider panel
  VTKWave.cpp            Loads the volumes, builds transfer functions, slicing and density logic
  voxel.cpp              Height-map → voxel volume (experimental)
  planeMesh.cpp          Height-map → triangle mesh (experimental)
  ArrowInteractorStyle.cpp  Arrow keys scroll the height map (experimental)
images/
  height_map.png         512 × 512 height map for the voxel experiment
  posx/negx/... .jpg     Cube-map skybox faces (loader present, not used)
```

## License

No license file has been added yet.
