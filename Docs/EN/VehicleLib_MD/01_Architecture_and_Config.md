# 01. Configuration Files, Schemas, and Tables in City Car Driving 2.0

**Navigation:** [README](./README.md) | [00. Quick Start](./00_Quick_Start.md) | **01. Architecture & Configs** | [02. Named I/O & States](./02_Named_IO_and_States.md) | [03. TorqueTrain](./03_Components_TorqueTrain.md) | [04. Electrics](./04_Components_Electrics.md) | [05. Chassis & Subsystems](./05_Components_Chassis_and_Subsystems.md)

---

## 1. Mod Files and Path Resolution in City Car Driving 2.0

In **City Car Driving 2.0**, the assets and configuration files for a modded vehicle reside in the mod plugin folder:
 `Plugins/Mods/Content/<ModName>/` (for example, `Plugins/Mods/Content/CieBensVGruppe/`).

The files in the mod folder include:

- **`V_<ModName>.uasset`** — vehicle Blueprint prototype (contains the 3D meshes for `Body`, `Wheel*`, `Suspension*`, lights, dashboard, and their physics/controller settings).
- **`DA_<ModName>.uasset`** — vehicle data asset card for the vehicle selection menu (maps UI trims/configurations to options in `options.xml`).
- **`ST_<ModName>.uasset`** — localization string table (`StringTable`).
- **`Vehicle.ini`** — main configuration file containing parameters for all physical units.
- **`options.xml`** — configuration options definition (e.g., AT/MT selection and XML schema switching).
- **`Engine.csv`** — engine combustion torque and mechanical friction loss table.
- *(Optional)* custom XML schema files (`.xml`) and curve tables (`.csv`).

### Path Rules in `Vehicle.ini` and `options.xml`

All paths to schema files (`.xml`) and curve tables (`.csv`) written inside `Vehicle.ini` and `options.xml` are resolved **relative to the root `Content/` directory of the base City Car Driving 2.0 game project**:

1. **Referencing shared base-game resources (`Content/Common/Vehicles/MotorCars/...`):**
   Written directly without a leading slash:
   ```ini
   DefaultSchema = Common/Vehicles/MotorCars/Schemes/AwdAuto.xml
   ProgressionCurve = Common/Vehicles/MotorCars/Tables/SpringProgression.csv
   ForceCurveLateral = Common/Vehicles/MotorCars/Tables/WheelForceLat.csv
   ```
2. **Referencing custom files inside the mod folder (`Plugins/Mods/Content/<ModName>/...`):**
   To navigate out of the base project's `Content/` folder into the plugin folder `Plugins/Mods/Content/<ModName>/`, use the relative path `../Plugins/Mods/Content/<ModName>/...`:
   ```ini
   PowerCurves = ../Plugins/Mods/Content/CieBensVGruppe/Engine.csv
   ```

> [!NOTE]
> When creating a new mod, make sure the `PowerCurves` parameter in the `[Engine]` section points to the `Engine.csv` file inside your mod folder (`../Plugins/Mods/Content/<ModName>/Engine.csv`).

---

## 2. `Vehicle.ini` Syntax and the `options.xml` System

### 2.1. The `[Vehicle]` Section and Component Sections `[ComponentName]`

`Vehicle.ini` is case-sensitive (for both section headers and parameter keys) and must be saved in **UTF-8** encoding.

- The system section **`[Vehicle]`** contains only one parameter — **`DefaultSchema`** (the default XML schema path used when no option in `options.xml` overrides the schema):
   ```ini
   [Vehicle]
   DefaultSchema = Common/Vehicles/MotorCars/Schemes/AwdAuto.xml
   ```
- Every other section **`[ComponentName]`** corresponds to the `Name="..."` attribute of a specific component in the XML schema (for example, `[Engine]`, `[Idler]`, `[GearBoxAutomatic]`, `[SuspensionFrontLeft]`).

> [!IMPORTANT]
> **Where are vehicle mass and center of mass configured?**
> `Vehicle.ini` has no parameters for chassis mass or center of mass — they are configured directly on the root physical **`Body`** component (`StaticMeshComponent`) inside the vehicle Blueprint `V_<ModName>.uasset` in the Unreal Engine 5 editor:
>
> 1. **Sprung mass (`Override Mass`):** in the **Physics** section of `Body`, be sure to enable **`Override Mass`** and enter the sprung body mass in **`Mass in Kg`** (curb mass minus the mass of all four wheels: $M_{\text{body}} = M_{\text{curb}} - 4 \cdot M_{\text{wheel}}$). If `Override Mass` is left disabled, UE5 computes mass from the collision volume using the nonlinear formula $1000 \cdot V^{0.75}$, which underestimates the mass of large vehicles by almost half.
> 2. **Center of mass offset (`Center Of Mass Offset` / `COMNudge`):** configured under **Physics** in **centimeters** as a relative offset $(\Delta X, \Delta Y, \Delta Z)$ from the automatically calculated geometric centroid of the body collision hulls ($\text{COM}_{\text{final}} = \text{COM}_{\text{auto}} + \text{Center Of Mass Offset}$).
> 3. **Collision physical material:** the `Body` `StaticMesh` asset must have a **`Simple Collision Physical Material`** assigned, and all other components and meshes in the Blueprint (wheels, suspensions, windows, interior parts) must use the **`NoCollision`** collision preset.

### 2.2. The `options.xml` Trim System and Section Overrides in `Vehicle.ini`

The `options.xml` file in the mod folder allows a single `Vehicle.ini` to support multiple vehicle configurations (for example, AT and MT versions or multiple engine variants):

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<Options>
    <AvailableOptions>
        <!-- Only ONE option in the file may have SetSchema="true" -->
        <Option Name="TransmissionType" SetSchema="true">
            <OptionValue Value="Auto"   SchemaName="Common/Vehicles/MotorCars/Schemes/AwdAuto.xml" />
            <OptionValue Value="Manual" SchemaName="Common/Vehicles/MotorCars/Schemes/AwdManual.xml" />
        </Option>

        <!-- Additional engine option (without switching XML schema) -->
        <Option Name="EngineType" SetSchema="false">
            <OptionValue Value="1.6TD" />
            <OptionValue Value="1.8I" />
        </Option>
    </AvailableOptions>

    <!-- Optional block disabling incompatible combinations -->
    <Compatibilities>
        <Compatibility FirstOptionType="EngineType" FirstOptionValue="1.6TD"
                       SecondOptionType="TransmissionType" SecondOptionValue="Manual"
                       Enable="false" />
    </Compatibilities>
</Options>
```

> [!TIP]
> **How to configure both transmissions (AT/MT) and two engine types (Gasoline/Diesel) at once:**
> Because `SetSchema="true"` in `options.xml` can only be placed on **one** option, bind `SetSchema="true"` to transmission selection (`TransmissionType`: `*Auto.xml` and `*Manual.xml`), and use `Class="EngineGazoline"` from the standard schemas as a universal base ICE. The engine type (gasoline or diesel) is then fully defined through `[Engine.EngineType.1.8I]` and `[Engine.EngineType.1.6TD]` sections in `Vehicle.ini` using different `PowerCurves` torque tables, operating rev limits `RpmLimit`, and idle settings `[Idler.EngineType.*]` (see [Section 2.1 of document 03](./03_Components_TorqueTrain.md) for details).

To override individual parameters of a component only when a specific option is selected, add a section of the form **`[ComponentName.OptionName.OptionValue]`** to `Vehicle.ini`:

```ini
[DifferentialAxle.TransmissionType.Manual]
Inertia = 1e-4
GearRatio = 3.45
Locked = false

[DifferentialAxle.TransmissionType.Auto]
Inertia = 1e-4
GearRatio = 3.267
Locked = false

[Speedometer.TransmissionType.Manual]
MetersPerRotation = 0.6408

[Speedometer.TransmissionType.Auto]
MetersPerRotation = 0.6768
```

When the vehicle loads, keys are read from the base `[ComponentName]` section first, and then keys from any active `[ComponentName.OptionName.OptionValue]` sections are applied on top.

### 2.3. Critical `Vehicle.ini` Syntax Rules and Limits

> [!CAUTION]
> **Two primary `.ini` rules whose violation breaks physics loading:**
>
> 1. **The separator for lists and 3D vectors is strictly a semicolon `;` with NO space before `;`!**
>    Commas `,` are **not supported** for separating numbers in vectors (`Mount`) or arrays (`Gears`, `ShiftAccelerator`, `OptimalRpm`). Furthermore, a **"space before `;`" sequence (`" ;"`)** is treated by the `.ini` parser as the beginning of an end-of-line comment, truncating all remaining values!
>    - **CORRECT** (no space before `;`; spaces or tabs *after* `;` are allowed):
>       ```ini
>       Mount = 1.63; 0.85; 0.4455
>       Gears = -3.416; 4.377; 2.859; 1.921; 1.368; 1.000; 0.820; 0.728
>       ```
>    - **ERROR** (commas or a space before `;`):
>       ```ini
>       Mount = 1.63, 0.85, 0.4455     ; ERROR: commas are not parsed!
>       Mount = 1.63 ; 0.85 ; 0.4455   ; ERROR: space before ';' cuts off 0.85 and 0.4455!
>       ```
> 2. **Section header and line length limits:**
>    - The section name length inside square brackets `[...]` (including `.OptionName.OptionValue`) **must not exceed 49 characters** (`50` bytes including null terminator). Longer headers are truncated and the option override fails to match.
>    - A single line in `Vehicle.ini` **must not exceed 400 characters**.

| Parameter Type in `.ini` | Formatting Rules | Example |
| --- | --- | --- |
| **Floating-point number (`Real`)** | Decimal point `.` (not a comma!). Scientific notation `1e-4` is supported. | `SpringCoeff = 45000.0` |
| **Integer (`Integer`)** | Signed integer value. | `StartingGear = -1` |
| **Boolean flag (`Boolean`)** | `true` / `false` (also `1` / `0`, `yes` / `no`, `on` / `off`). | `Locked = false` |
| **String (`String`)** | Path or name **without quotes**. | `BodyName = Body` |
| **3D vector (`Vector3`)** | Exactly 3 numbers separated by `;` with no space before `;`. | `Mount = -1.59; -0.85; 0.4589` |
| **Numeric array** | Ordered numbers separated by `;` with no space before `;`. In paired lookup arrays (`ShiftAccelerator`, `OilPressureRPM`), key values must be strictly ascending. | `OilPressureRPM = 0; 750; 4600` |

---

## 3. Standard Passenger Car XML Schemas in City Car Driving 2.0

The directory `Common/Vehicles/MotorCars/Schemes/` provides **6 ready-made schemas** covering every combination of drivetrain layout and transmission type:

- **All-Wheel Drive (AWD):** `AwdAuto.xml` (AT), `AwdManual.xml` (MT)
- **Front-Wheel Drive (FWD):** `FwdAuto.xml` (AT), `FwdManual.xml` (MT)
- **Rear-Wheel Drive (RWD):** `RwdAuto.xml` (AT), `RwdManual.xml` (MT)

### 3.1. Complete List of `[ComponentName]` Sections in `Vehicle.ini` for Standard Schemas

| Subsystem | Sections in `Vehicle.ini` (`[Name]`) | Class (`Class` in XML) | Present in Schemas |
| --- | --- | --- | --- |
| **Engine, fuel, and cooling** | `[Engine]`<br>`[Idler]`<br>`[FuelTank]` | `EngineGazoline`<br>`FuelControllerComputer`<br>`Tank` | All 6 schemas |
| **Ignition switch and cranking (starter)** | `[IgnitionKey]`<br>`[Starter]`<br>`[StarterRelay]`<br>`[StarterGear]`<br>`[StarterBendix]` | `IgnitionKeyProcessor`<br>`EngineElectric`<br>`Relay`<br>`Reducer`<br>`Bendix` | All 6 schemas |
| **Onboard electrical system and dashboard** | `[Battery]`<br>`[Generator]`<br>`[BatteryElectricCircuit]`<br>`[GeneratorElectricCircuit]`<br>`[IgnitionElectricCircuit]`<br>`[CarElectrics]` | `Battery`<br>`Generator`<br>`ElectricCircuit`<br>`ElectricCircuit`<br>`ElectricCircuit`<br>`CarElectrics` | All 6 schemas *(no `.ini` sections are needed for `[*ElectricCircuit]`)* |
| **Transmission: AT (`*Auto.xml`)** | **`[GearBoxAutomatic]`**<br>`[TorqueConverter]`<br>`[GearBoxLogic]`<br>`[GearBoxAutomaticPump]`<br>`[OilTank]` | `GearBoxAutomatic`<br>`HydraulicTorqueConverter`<br>`GearBoxAutomaticLogic`<br>`Pump`<br>`Tank` | **`AwdAuto`, `FwdAuto`, `RwdAuto` only** |
| **Transmission: MT (`*Manual.xml`)** | **`[GearBox]`**<br>`[Clutch]`<br>`[ClutchPedal]` | `GearBox`<br>`ClutchFriction`<br>`ClutchInputProcessor` | **`AwdManual`, `FwdManual`, `RwdManual` only** |
| **Differentials: AWD (`Awd*`)** | **`[DifferentialAxle]`** *(center)*<br>**`[FrontAxisRelay]`** *(front-axle coupling)*<br>**`[DifferentialWheelFront]`** *(front)*<br>**`[DifferentialWheelBack]`** *(rear)* | `DifferentialOpen`<br>`Relay`<br>`DifferentialOpen`<br>`DifferentialOpen` | **`AwdAuto` and `AwdManual` only** *(speedometer is linked to `DifferentialAxle`)* |
| **Differential: 2WD (`Fwd*`, `Rwd*`)** | **`[Differential]`** | `DifferentialOpen` | **`FwdAuto`, `FwdManual`, `RwdAuto`, `RwdManual` only** |
| **Suspension (4 corners)** | `[SuspensionFrontLeft]`<br>`[SuspensionFrontRight]`<br>**`[SuspensionBackLeft]`**<br>**`[SuspensionBackRight]`** | `Suspension1Spring` | All 6 schemas |
| **Wheels / tires (4 pcs.)** | `[WheelFrontLeft]`<br>`[WheelFrontRight]`<br>**`[WheelBackLeft]`**<br>**`[WheelBackRight]`** | `WheelRaycast` | All 6 schemas |
| **Brake mechanisms (4 pcs.) + ABS** | `[BrakeFrontLeft]`<br>`[BrakeFrontRight]`<br>**`[BrakeBackLeft]`**<br>**`[BrakeBackRight]`**<br>`[AbsTcs]` | `BrakeFriction`<br>`BrakeFriction`<br>`BrakeFriction`<br>`BrakeFriction`<br>`AbsTcs` | All 6 schemas |
| **Steering, power steering/FFB, speedometer, body** | `[Steering]`<br>`[PowerSteering]`<br>`[Speedometer]`<br>`[Aerodynamics]` | `Steering2Ackermann`<br>`PowerSteering`<br>`Speedometer`<br>`AerodynamicsCarSimple` | All 6 schemas |

> [!WARNING]
> **Four critical naming conventions for sections and meshes in City Car Driving 2.0:**
>
> 1. **The rear axle uses `Back`, NOT `Rear` (mandatory for Blueprint and standard schemas):**
>    - In the vehicle prototype Blueprint (`V_<ModName>`) and XML schema, the visual and physical wheel and hub nodes **must** be named `SuspensionFrontLeft/Right`, **`SuspensionBackLeft/Right`**, `WheelFrontLeft/Right`, **`WheelBackLeft/Right`** — the game uses these exact names to merge mod components with the base vehicle template and bind 3D wheel meshes to physics.
>    - In the standard XML schemas, the rear brake sections are likewise named **`[BrakeBackLeft]`** and **`[BrakeBackRight]`** (writing `Rear` in `Vehicle.ini` will fail to match the XML schema and leave default values active).
> 2. **Different differential section names in standard AWD vs FWD/RWD schemas:**
>    In the built-in `AwdAuto.xml` and `AwdManual.xml` schemas, the center differential is named **`[DifferentialAxle]`**, and the axle differentials are `[DifferentialWheelFront]` and `[DifferentialWheelBack]`. In 2WD schemas (`Fwd*.xml`, `Rwd*.xml`), the single differential is named **`[Differential]`**. *(In a custom XML schema you can name a differential freely, as long as `Name` in XML matches `[Name]` in `Vehicle.ini`.)*
> 3. **Different AT and MT section names in standard schemas:**
>    In the standard `*Auto.xml` schemas the gearbox is named **`[GearBoxAutomatic]`**, whereas in `*Manual.xml` it is named **`[GearBox]`**. *(In a custom XML schema the section name is determined by the component's `Name` attribute in XML.)*
> 4. **Physical chassis mesh name is `Body` (`BodyName = Body`):**
>    The root physical `StaticMeshComponent` of the chassis in the vehicle template Blueprint must be named **`Body`**. In `[Suspension*]` and `[Aerodynamics]`, `BodyName` already defaults to `"Body"`, so writing `BodyName = Body` explicitly is good practice for clarity (you must not specify any other name that does not exist among the Blueprint's physical meshes/bones).

---

## 4. Structure of a `<Vehicle>` XML Schema and Creating a Custom Mod Schema

Every vehicle XML schema file has the root tag **`<Vehicle>`** and consists of 6 sections:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Vehicle>
    <!-- 1. Component list: Name defines the [Name] section in Vehicle.ini, Class defines the unit type -->
    <Components>
        <Component Name="Turbocharger" Class="Turbocharger" />
        <Component Name="Engine" Class="EngineDiesel" />
        <Component Name="Idler" Class="FuelControllerComputer" />
    </Components>
    <!-- 2. Directed links between components (from owner/driver Holder to driven Reference) -->
    <Links>
        <Link Holder="Turbocharger" Reference="Engine" />
        <Link Holder="IgnitionKey" Reference="Idler" />
        <Link Holder="BatteryElectricCircuit" Reference="IgnitionElectricCircuit" Connected="false" />
        <Link Holder="CarElectrics" Reference="GearBox" StartIconGearBox="true" />
    </Links>
    <!-- 3. Telemetry and dashboard output channels -->
    <OutputElements>
        <Element Name="RPM" Type="Real" Component="Engine" />
        <Element Name="DashboardRPM" Type="Real" Component="CarElectrics" InternalName="RPM" />
    </OutputElements>
    <!-- 4. Internal states for instant initialization from the game -->
    <StateElements>
        <Element Name="FuelVolume" Type="Real" Component="FuelTank" InternalName="LiquidVolume" />
    </StateElements>
    <!-- 5. External control input channels -->
    <InputElements>
        <Element Name="Accelerator" Type="Real" />
        <Element Name="Ignition" Type="Integer" />
    </InputElements>
    <!-- 6. Routing of input channels to components -->
    <InputProcessors>
        <Processor Input="Accelerator" Component="AbsTcs" />
        <Processor Input="Accelerator" Component="Turbocharger" InternalName="Throttle" />
        <Processor Input="Ignition" Component="IgnitionKey" />
    </InputProcessors>
</Vehicle>
```

### Step-by-Step Recipe: Converting a Schema to a True `EngineDiesel` with `Turbocharger`

All 6 standard schemas in `Common/Vehicles/MotorCars/Schemes/` use the gasoline `EngineGazoline` class without a turbocharger. To build a full turbodiesel for your mod:

1. **Copy a base schema** (e.g., `AwdAuto.xml` and/or `AwdManual.xml`) from `Content/Common/Vehicles/MotorCars/Schemes/` into your mod folder `Plugins/Mods/Content/<ModName>/`.
2. **In `<Components>`**, replace the engine with `<Component Name="Engine" Class="EngineDiesel" />` and add `<Component Name="Turbocharger" Class="Turbocharger" />` (before `Idler` and `Engine`).
3. **In `<Links>`, be sure to remove `<Link Holder="IgnitionElectricCircuit" Reference="Engine" />`** (a diesel engine does not use a spark ignition circuit; leaving this link will prevent the schema from loading!).
4. **In `<Links>`, keep `<Link Holder="IgnitionKey" Reference="Idler" />`** (this cuts fuel supply when the key is turned to `0` to shut down the diesel engine) and add **`<Link Holder="Turbocharger" Reference="Engine" />`**.
5. **In `<InputProcessors>`**, add **`<Processor Input="Accelerator" Component="Turbocharger" InternalName="Throttle" />`**.
6. **Point `options.xml` and `Vehicle.ini` to your local schemas** (`../Plugins/Mods/Content/<ModName>/AwdAuto.xml`) and add a `[Turbocharger]` section to `Vehicle.ini` (`MinRpm`, `RefRpm`, `RefCompressionRatio`, `LagTime = 0.08..0.10`).

---

## 5. `.csv` Curve Table Formats and `Engine.csv` Structure

In all `.csv` files, the cell delimiter is a **semicolon `;`** and the decimal separator is **strictly a dot `.`** (a comma such as `8,8` causes a read error!). Argument nodes must be strictly ascending with no duplicates.

### 5.1. Standard Shared Tables in `Common/Vehicles/MotorCars/Tables/`

| Table File in `Common/Vehicles/MotorCars/Tables/` | Format | Where Referenced in `Vehicle.ini` and What It Defines |
| --- | :-: | --- |
| **`SpringProgression.csv`** | 1D (2 rows) | `[Suspension*]` $\rightarrow$ `ProgressionCurve`: relative spring compression `[0..1]` $\rightarrow$ stiffness multiplier `[1.0..2.5]`. |
| **`EngineVibration.csv`** | 1D (2 rows) | `[Engine]` $\rightarrow$ `VibrationCurve`: normalized engine speed `RPM / IdleRpm` $\rightarrow$ ICE vibration amplitude. |
| **`WheelForceLat.csv`** | 1D (2 rows) | `[Wheel*]` $\rightarrow$ `ForceCurveLateral`: tire lateral slip angle (deg., `[0..180]`) $\rightarrow$ lateral force at `ForceCurveLoad`. |
| **`WheelForceLon.csv`** | 1D (2 rows) | `[Wheel*]` $\rightarrow$ `ForceCurveLongitudinal`: tire longitudinal slip (`%`, `[0..200]`) $\rightarrow$ longitudinal force at `ForceCurveLoad`. |
| **`WheelAlignTorque.csv`** | 1D (2 rows) | `[Wheel*]` $\rightarrow$ `AligningTorqueCurve`: slip angle (deg.) $\rightarrow$ tire self-aligning torque. *Note: at the peak of `WheelForceLat.csv` (`10°`), the value in `WheelAlignTorque.csv` must be strictly `0`!* |
| **`WheelStaticAligningTorque.csv`** | 1D (2 rows) | `[Wheel*]` $\rightarrow$ `StaticAligningTorqueCurve`: normalized angle `[-1..1]` $\rightarrow$ static self-centering torque at standstill. |
| **`WheelRollingResistance.csv`** | 2D (matrix) | `[Wheel*]` $\rightarrow$ `RollingResistanceCurves`: row 1 — relative tire pressure `P / WorkingPressure` (`[0..1.5]`); column 1 — surface looseness (`[0..1]`); cells — rolling resistance multiplier. |
| **`WheelSagToCamberCurve.csv`** | 1D (2 rows) | `[Suspension*]` $\rightarrow$ `SagToCamberCurve`: suspension compression `[0..1]` $\rightarrow$ base wheel camber angle (deg.). |
| **`WheelSteerToCamberCurve.csv`** | 1D (2 rows) | `[Suspension*]` $\rightarrow$ `SteerToCamberCurve`: wheel steer angle `[-30..30]` (deg.) $\rightarrow$ additional camber (deg.). |
| **`WheelCamberToSlipAngle.csv`** | 1D (2 rows) | `[Wheel*]` $\rightarrow$ `CamberToSlipAngleCurve`: camber angle `[-5..5]` (deg.) $\rightarrow$ slip angle offset (deg.). |

### 5.2. One-Dimensional Table Format (1D `.csv`)

Consists of exactly **2 rows** with the same number of columns (at least 2 points):

- **Row 1:** argument values $X_0; X_1; \dots; X_{n-1}$ in ascending order.
- **Row 2:** function values $Y_0; Y_1; \dots; Y_{n-1}$.

```csv
0;0.55;0.7;0.85;0.95;1
1;1;1.06;1.3;1.7;2.5
```

### 5.3. Exact Format of the Engine Table `Engine.csv` (`PowerCurves`)

The `Engine.csv` file, referenced via the `PowerCurves` parameter in `[Engine]`, combines a 2D map of indicated combustion torque with a 1D curve of mechanical friction losses (engine braking):

1. **Row 1 (`RPM` header):** the first cell is the text `thr/rpm` (or `0`), followed by ascending `RPM` values (for example, `0; 50; 250; 500; ...; 5500`).
2. **Rows 2..M (torque map by throttle pedal position):**
   - The first cell of each row is the **accelerator pedal percentage from `0` to `100`** (`0; 5; 10; 20; 40; 60; 80; 100`). On load, these values are automatically divided by `100.0` to become throttle opening fractions `[0.0 .. 1.0]`.
   - The remaining cells in the row are the indicated combustion torque in **$\text{N}\cdot\text{m}$** at the corresponding RPM.
3. **Last row (immediately after the `100` row):**
   - The first cell is `0` (or `Friction`), and the remaining cells are the **positive internal mechanical loss (engine braking) torque $F(\text{RPM}) > 0$ in $\text{N}\cdot\text{m}$** for each RPM column. The last column beyond the rev limit (`5500` or `8000 RPM`) typically uses a barrier value of `10000`.

```csv
thr/rpm;0;50;250;500;750;1000;1500;2000;2500;3000;3500;4000;4500;5500
0;0;0;0;0;0;0;0;0;0;0;0;0;0;0
5;0;15;25;150;32;24;20;18;16;14;12;10;6;0
10;0;18;38;180;58;46;42;38;35;31;27;22;14;0
20;0;20;50;162;115;105;98;92;85;76;66;54;34;0
40;0;20;62;168;172;178;175;165;152;138;122;100;65;1
60;0;20;70;174;212;228;222;200;180;162;142;116;76;1.5
80;0;20;76;180;242;252;240;202;182;162;141;115;78;2
100;0;20;80;184;260;265;242;202;184;162;141;122;88;2.5
0;12;15;20;24;28;35;48;58;68;78;88;98;110;10000
```

> [!IMPORTANT]
> **Three essential rules for calculating `Engine.csv`:**
>
> 1. **Friction row subtraction from every row:** On every simulation step, the engine physics model subtracts the last mechanical loss row $F(\text{RPM})$ from combustion torque **even at 100% throttle**:
>    $$T_{\text{flywheel}}(\text{Throttle}, \text{RPM}) = T_{\text{table}}(\text{Throttle}, \text{RPM}) - F(\text{RPM}).$$
>    Therefore, to obtain the rated flywheel torque $T_{\text{DIN}}(\text{RPM})$ (e.g., $440\text{ N}\cdot\text{m}$) on a naturally aspirated engine, the `100` row must contain the **sum** $T_{100}(\text{RPM}) = T_{\text{DIN}}(\text{RPM}) + F(\text{RPM})$.
> 2. **Recalculating the table when using `Turbocharger`:** If a `Turbocharger` component is connected to the engine, it multiplies the table's combustion torque (before friction subtraction!) by the boost ratio:
>    $$\text{ratio}(\text{RPM}, \text{thr}) = 1 + (\text{RefCompressionRatio} - 1)\sqrt{\frac{\max(0,\; \text{RPM}\cdot\text{thr} - \text{MinRpm})}{\text{RefRpm} - \text{MinRpm}}}.$$
>    Thus, when `Turbocharger` is present, each cell in rows `0..100` (with $\text{thr} = \text{percent}/100$) is computed as $\big(T_{\text{target}}(\text{RPM}, \text{thr}) + F(\text{RPM})\big) / \text{ratio}(\text{RPM}, \text{thr})$, while the last friction row $F(\text{RPM})$ is left un-divided!
> 3. **Holding idle under load ("anti-stall hump" — `[Idler]` simulation quirk):** In the current implementation, the `FuelControllerComputer` (`[Idler]`) idle controller operates open-loop — it chooses a throttle position solely to offset the engine's own internal friction $F(\text{IdleRpm})$ and does not add throttle when RPM sags below `IdleRpm` under external load (torque converter in `D`, clutch engagement, or alternator load). Therefore, in `Engine.csv`, the `5%` row at the `IdleRpm` column (`750 RPM`) is set slightly above friction $F(750)$, while the adjacent lower column (`500 RPM`) is given a steep torque rise (an "anti-stall hump" across the `5%–20%` rows) so that torque rises sharply whenever RPM dips under load, preventing stalls.

---

## 6. Tractor-Trailer Coupling (`<VehicleAttachment>`)

To dynamically link a tractor (`Master`) and trailer (`Slave`), a separate coupling schema XML file with the root tag **`<VehicleAttachment>`** is used:

```xml
<?xml version="1.0" encoding="utf-8"?>
<VehicleAttachment>
    <!-- MasterLinks: Holder is resolved in the tractor schema, Reference in the trailer schema -->
    <MasterLinks>
        <Link Holder="BatteryElectricCircuit" Reference="TrailerElectricCircuit" Connected="true" />
        <Link Holder="TractorMultiCircuitValve" Reference="TrailerAirReceiver" />
    </MasterLinks>

    <!-- SlaveLinks: Holder is resolved in the trailer schema, Reference in the tractor schema -->
    <SlaveLinks>
    </SlaveLinks>
</VehicleAttachment>
```

When coupled, all identically named control input channels (`Brake`, `ParkingBrake`, `LeftTurnLight`, `RightTurnLight`, `HazardLight`, `ParkingLights`, `LowBeam`) are automatically transmitted in sync from the tractor to every attached trailer section.
