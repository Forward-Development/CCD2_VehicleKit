# Vehicle Modding Guide for City Car Driving 2.0

**Navigation:** **README** | [01. Minimal Car](./01_Minimal_Car_QuickStart.md) | [02. Exterior, Lights & World](./02_Exterior_Lights_and_World.md) | [03. Interior, Dashboard & Cameras](./03_Interior_Dashboard_Cameras_Seats.md) | [04. Mirrors & Wipers](./04_Mirrors_and_Wipers.md) | [05. Sounds & FMOD](./05_Sounds_and_FMOD.md) | [06. Specs, Logic & Reference](./06_Advanced_Specs_Logic_and_Reference.md)

---

Welcome to the step-by-step guide for creating custom vehicle mods for **City Car Driving 2.0** using **Unreal Engine 5.8**.

This documentation is written so that **any modder can follow it — even if you have never opened Unreal Engine before**. It is organized into progressive modules by priority: starting with **Module 1**, you will build, package, and drive a minimal working vehicle (a body + 4 wheels) in-game, and then upgrade it step by step into a complete, polished mod.

---

## 1. Module Roadmap

```mermaid
flowchart LR
    M0["README<br/>Setup & UE Basics"] --> M1["Module 1 (MVP)<br/>Body + 4 Wheels + .pak"]
    M1 --> M2["Module 2<br/>Exterior, Lights & World"]
    M2 --> M3["Module 3<br/>Interior, Dashboard & Cameras"]
    M3 --> M4["Module 4<br/>Mirrors & Wipers"]
    M4 --> M5["Module 5<br/>Sounds & FMOD"]
    M5 --> M6["Module 6<br/>Specs, Physics & Reference"]
```

| # | Module | Priority | Outcome After Completing |
| :-: | --- | --- | --- |
| **—** | **[README.md](./README.md)** *(you are here)* | Required | UE 5.8 installed, template project opened, core UE editor concepts and navigation understood. |
| **01** | **[01. Minimal Car (Quick Start)](./01_Minimal_Car_QuickStart.md)** | **Required (MVP)** | A packaged `.pak` containing a minimal working car (body, 4 wheels, basic camera, physics, menu specification), tested locally in-game and ready for Steam Workshop. |
| **02** | **[02. Exterior, Lights, Suspension & World Interaction](./02_Exterior_Lights_and_World.md)** | Recommended | Full exterior PBR materials, non-rotating brake calipers (`Suspension*`), emissive lamps & real light sources, license plates, refueling (`FuelTank`), world interaction (`LogicBox`), and rain blocking (`RainBlocker`). |
| **03** | **[03. Interior, Dashboard, Cameras & Driver Seating](./03_Interior_Dashboard_Cameras_Seats.md)** | Recommended | Cabin interior, rotating steering wheel, working dashboard needles & warning lights, all 5 game cameras, driver seating, and cabin toy mount. |
| **04** | **[04. Mirrors & Wipers](./04_Mirrors_and_Wipers.md)** | Recommended | Working rear-view mirrors with render cameras and animated windshield wipers. |
| **05** | **[05. Vehicle Sounds & FMOD Studio](./05_Sounds_and_FMOD.md)** | Optional | Positioning & picking built-in game sounds, or creating completely custom sound banks in FMOD Studio. |
| **06** | **[06. Configurations, Physics, Custom Logic & Reference](./06_Advanced_Specs_Logic_and_Reference.md)** | Advanced | Vehicle card stats, switchable options (`options.xml`), deep physics tuning (`VehicleLib`), custom Blueprint logic, and the all-in-one cheat sheet. |

> [!TIP]
> **Dedicated Physics Tuning Reference:**
> Alongside this guide sits the complete engineering reference for vehicle physics — **[VehicleLib_MD](../VehicleLib_MD/README.md)**. In Module 1 we only configure the bare minimum needed to get your car driving; for fine-tuning engines, gearboxes, suspension, and tires, refer to `VehicleLib_MD`.

---

## 2. Software Requirements & Downloads

1. **Unreal Engine 5.8** — download and install via **Epic Games Launcher** (*Unreal Engine → Library → Engine Versions → `+` → 5.8*).
   - A standard **UE 5.8 installation from the Epic Games Store is all you need** to create and package mods (including running `build_mod_dlc.bat`) — you do not need to install Visual Studio or extra SDKs.
2. **Template project `CCD2_VehicleKit`** — download from GitHub:
   - <https://github.com/Forward-Development/CCD2_VehicleKit/tree/main>
   - Extract the project to a **short path using Latin characters only and no spaces**, e.g. `D:\Modding\CCD2_VehicleKit\`.
3. **3D Editor** (Blender, 3ds Max, Maya, or any software capable of `.fbx` export).
4. *(Module 5 only, if you want fully custom sounds)* **FMOD Studio 2.03.14** and template projects:
   - <https://github.com/Forward-Development/CCD2_VehicleKit_FMOD>

> [!IMPORTANT]
> **Never use non-Latin characters or spaces in your project path or mod folder/file names!**
> Use paths like `D:\Modding\CCD2_VehicleKit\` and mod folder names like `MyFirstCar`. This avoids `.pak` packaging failures.

---

## 3. First Launch & Unreal Engine 5.8 Mini-Guide for Beginners

If this is your first time using Unreal Engine, this section will save you hours.

### 3.1. Opening the Project
1. Open the extracted `CCD2_VehicleKit` folder and double-click **`CityCarDriving2.uproject`**.
2. On the **first launch**, the editor may take 5–15 minutes to open and display `Compiling Shaders (...)` in the bottom-right corner. This is normal — let it finish; subsequent launches will be fast.

### 3.2. Key Editor Panels & Controls
- **Content Browser / Content Drawer** (`Ctrl + Space`): the file manager for your UE project. All 3D meshes, textures, materials, Blueprints, and data assets live here.
- **Viewport** (large center 3D view):
  - Hold **Right Mouse Button (RMB)** and press **`W` / `A` / `S` / `D`** (move) or **`Q` / `E`** (down/up) to fly around.
  - Scroll the **mouse wheel** while holding RMB to adjust camera flight speed.
  - Press **`F`** to focus instantly on the currently selected object.
  - Press **`W` / `E` / `R`** (without holding RMB) to switch between **Move / Rotate / Scale** gizmos.
- **Details** (right-hand panel): displays all properties of the selected object or component. Use the **Search** bar at the top of Details to jump directly to any property (e.g., `Asset User Data`, `Collision`, `Virtual`).
- **Saving (`Ctrl + Shift + S`):** Unreal Engine does not auto-save asset edits to disk! Assets with a small asterisk `*` on their icon have unsaved changes. Always run **File → Save All** (`Ctrl + Shift + S`) before packaging your mod.

### 3.3. Mandatory Setting: Showing the Mods Folder (`Show Plugin Content`)
By default, Unreal Engine hides plugin folders. To see the Mods folder:
1. Open **Content Browser** (`Ctrl + Space`).
2. Click **Settings** (gear icon in the top-right corner of the Content Browser).
3. Check **Show Plugin Content**.
4. In the left folder tree, expand `All → Plugins → Mods Content`. All vehicle mods live here (including the sample vehicle `CieBensVGruppe`).

> [!TIP]
> **Opening a UE folder in Windows File Explorer:**
> Right-click any folder in the Content Browser and select **Show in Explorer**. You will need this to place text physics configs (`Vehicle.ini`, `options.xml`, `Engine.csv`) and audio banks (`AudioBanks/`), which do not appear inside the UE Content Browser.

### 3.4. Unreal Engine Glossary in Plain English

| UE Term | Plain-English Explanation |
| --- | --- |
| **Asset (`.uasset`)** | Any file inside UE: a 3D mesh, texture, material, vehicle Blueprint, or data table. |
| **Static Mesh (`SM_*`)** | A rigid 3D model without bones (body, wheels, interior, steering wheel, gauge needles). |
| **Skeletal Mesh (`SK_*`)** | A 3D model with a skeleton and animations (used for wipers in CCD 2.0 mods). |
| **Master Material / Material Instance (`MI_*`)** | A Master Material is a base shader. Custom Master Materials are **not supported** in mods — instead, you create a **Material Instance** (inheriting from a provided `MI_Mod_*` parent) and simply plug in your own textures and colors. |
| **Virtual Texture (`VT`)** | A streaming texture format required for all textures used in your mod. |
| **Blueprint Class (`Actor`, `V_*`)** | The container for your vehicle. Inside it, you assemble the body, wheels, cameras, lights, interior, and attach game properties. After editing a Blueprint, always click **Compile** (top-left) and **Save**. |
| **Component** | An individual element inside an Actor (shown in the left **Components** tree):<br>• `StaticMeshComponent` — renders a 3D mesh;<br>• `SceneComponent` — an invisible 3D point (used for cameras, sound emitters, fuel tank, or grouping);<br>• `BoxCollision` — an invisible bounding box (used for `LogicBox`). |
| **Collision** | Simplified physical geometry used for impacts with the road and obstacles. On a vehicle, **only `Body`** has physical collision. |
| **Pivot** | The origin point `(0, 0, 0)` of a 3D model around which it rotates. For example, a wheel's pivot must sit at its exact geometric center. |
| **Socket** | A named attachment point placed inside a 3D mesh (e.g., driver seat anchor `Seat0_Landing` or mirror capture direction `Camera`). |
| **ControlBlock (`AssetUserData`)** | CCD 2.0's metadata system attached to Actor components so the game knows *what role each component plays* (camera, light, steering wheel, needle, mirror, sound, etc.). |
| **Primary Data Asset (`PDA_*` / `DA_*`)** | Your vehicle's "passport" for the in-game menu: display name, preview image, class, engine/transmission types, and reference to the Actor. |

> [!TIP]
> **Helpful Editor Visual Cue (Green Highlighting):**
> The project includes the `SimulatorMinimal` editor plugin. As soon as you properly attach `Control Asset Data` (`ControlBlock`) to any component of your Actor, that component's name will automatically **turn bright green** in the **Components** tree! This makes it easy to spot which components are wired to game systems.

---

## 4. Mod Folder Structure & Sample Vehicle `CieBensVGruppe`

All files for your vehicle must reside inside a single folder:
`Plugins/Mods/Content/<ModName>/` (in Windows Explorer) or `Plugins/Mods Content/<ModName>/` (in UE Content Browser).

```text
Plugins/Mods/Content/<ModName>/
├── V_<ModName>.uasset         ← Vehicle Actor (Blueprint Class)
├── DA_<ModName>.uasset        ← Vehicle specification (PDA_PrototypedVehicleSpecification)
├── Vehicle.ini                ← Main physics parameters file (required)
├── options.xml                ← Configuration options & XML schema mapping (required)
├── Engine.csv                 ← Engine torque & friction curves (required)
├── AwdAuto.xml / ...          ← (Optional) Custom physics XML schemas
├── Meshes/                    ← Imported 3D models (body, wheels, interior...)
├── Materials/                 ← Material Instances created from MI_Mod_*
├── Textures/                  ← Textures (with Virtual Texture Streaming enabled)
├── Blueprints/                ← (Optional) Light components (SpotLight) & custom logic
└── AudioBanks/                ← (Optional) Custom FMOD sound banks (.bank + GUIDs.txt)
```

### Sample Vehicle `CieBensVGruppe`
A complete, fully configured example car is included at `Plugins/Mods/Content/CieBensVGruppe/`. You can:
- **Use it as a reference** (open `V_CieBensVGruppe` to inspect node hierarchy, `ControlBlock` entries, materials, light setups, and `Vehicle.ini`).
- **Or build your mod from scratch** in a new folder `Plugins/Mods/Content/<ModName>/` — this is the clean workflow followed in the modules below.

> [!NOTE]
> **You do NOT need to delete `CieBensVGruppe` before building your mod!**
> Inside the `CieBensVGruppe` folder there is a special ignore label named **`L_IGNORE`** — thanks to it, `CieBensVGruppe` will **not** be packed into your `.pak` when you run `build_mod_dlc.bat`.
> However, if you decide to build your own car *by editing or duplicating `CieBensVGruppe`*, in addition to renaming the folder and key assets you **must delete the `L_IGNORE` file** from your vehicle's folder — otherwise your car will be excluded from the `.pak`!

---

**Next step:** proceed to **[Module 1. Minimal Car (Quick Start)](./01_Minimal_Car_QuickStart.md)** to build and drive your first vehicle in-game.
