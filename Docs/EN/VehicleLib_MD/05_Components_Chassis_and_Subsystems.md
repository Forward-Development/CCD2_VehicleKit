# 05. Chassis and Subsystem Components (`Suspension`, `Input`, `Output`, `Pneumatics`, `Hydraulics`, `Aerodynamics`, `ExternalForce`)

**Navigation:** [README](./README.md) | [00. Quick Start](./00_Quick_Start.md) | [01. Architecture & Configs](./01_Architecture_and_Config.md) | [02. Named I/O & States](./02_Named_IO_and_States.md) | [03. TorqueTrain](./03_Components_TorqueTrain.md) | [04. Electrics](./04_Components_Electrics.md) | **05. Chassis & Subsystems**

---

## Table of Contents

- [1. Suspension and Steering](#1-suspension-and-steering)
   - [1.1. `Suspension1Spring` — Coil Spring and Damper Suspension](#11-suspension1spring--coil-spring-and-damper-suspension)
   - [1.2. `Suspension1Constant` — Rigid Constant-Load Suspension](#12-suspension1constant--rigid-constant-load-suspension)
   - [1.3. `Steering2Ackermann` — Ackermann Steering Linkage](#13-steering2ackermann--ackermann-steering-linkage)
   - [1.4. `SteeringSimple` — Symmetrical Steering](#14-steeringsimple--symmetrical-steering)
- [2. Control Input Processors (`Input`)](#2-control-input-processors-input)
   - [2.1. `FuelControllerComputer` (`[Idler]`) and `FuelControllerMechanical`](#21-fuelcontrollercomputer-idler-section-and-fuelcontrollermechanical)
   - [2.2. `AbsTcs` — ABS, TCS, and Electronic Brakeforce Distribution (EBD) Unit](#22-abstcs--abs-tcs-and-electronic-brakeforce-distribution-ebd-unit)
   - [2.3. `ClutchInputProcessor` — Nonlinear Clutch Pedal Controller](#23-clutchinputprocessor--nonlinear-clutch-pedal-controller)
   - [2.4. `GearBoxAutomaticLogic` (`[GearBoxLogic]`) and `GearBoxAutomaticLogicBus`](#24-gearboxautomaticlogic-gearboxlogic-section-and-gearboxautomaticlogicbus)
   - [2.5. `IgnitionKeyProcessor` — Ignition Switch Controller](#25-ignitionkeyprocessor--ignition-switch-controller)
- [3. Telemetry and Steering Force Feedback (`Output`)](#3-telemetry-and-steering-force-feedback-output)
   - [3.1. `PowerSteering` — Power Steering (HPS/EPS) and Force Feedback](#31-powersteering--power-steering-hpseps-and-force-feedback)
   - [3.2. `Speedometer` — Speedometer and Odometer](#32-speedometer--speedometer-and-odometer)
   - [3.3. `FuelConsumptionMeter` — Average Fuel Consumption Meter](#33-fuelconsumptionmeter--average-fuel-consumption-meter)
- [4. Forced Induction, Pneumatics, Tanks, Aerodynamics, and External Forces](#4-forced-induction-pneumatics-tanks-aerodynamics-and-external-forces)
   - [4.1. `Turbocharger` — Engine Forced-Induction Turbocharger](#41-turbocharger--engine-forced-induction-turbocharger)
   - [4.2. Pneumatic System: `Receiver`, `ValveMultiCircuit`, `ActuatorPneumatic1/2Chamber`](#42-pneumatic-system-receiver-valvemulticircuit-actuatorpneumatic1chamber-actuatorpneumatic2chamber)
   - [4.3. `Tank` — Fuel and Oil Tank](#43-tank--fuel-and-oil-tank)
   - [4.4. `AerodynamicsCarSimple` — Body Aerodynamics](#44-aerodynamicscarsimple--body-aerodynamics)
   - [4.5. `ExternalThruster` — External Linear Thruster](#45-externalthruster--external-linear-thruster)

---

## General Configuration Rules in `Vehicle.ini` and XML Schematics

1. **Strict array and vector syntax in `Vehicle.ini`:** all number lists and 3D vectors must be separated **only by a semicolon `;` with no space before `;`** (`Mount = 1.63; 0.85; 0.4455`). Commas `,` are not supported, and a `space + ;` sequence (`" ;"`) is treated by the `.ini` parser as the start of a comment, truncating the rest of the line!
2. **Connections (`<Links>`) and signals in XML:** links between components are defined via `<Link Holder="HolderName" Reference="TargetName" />`. External inputs, outputs, and states are mapped to internal channel names (`InternalName`) in the `<InputProcessors>`, `<OutputElements>`, and `<StateElements>` sections.

---

## 1. Suspension and Steering

### 1.1. `Suspension1Spring` — Coil Spring and Damper Suspension

In standard CCD 2.0 schematics (`Common/Vehicles/MotorCars/Schemes/*.xml`), this component is used for all four suspension struts of a vehicle: **`[SuspensionFrontLeft]`**, **`[SuspensionFrontRight]`**, **`[SuspensionBackLeft]`**, **`[SuspensionBackRight]`** (the rear axle is always named **`Back`**, never `Rear`!).

- **XML links (`<Links>`):** each strut links to its corresponding wheel (`<Link Holder="SuspensionFrontLeft" Reference="WheelFrontLeft" />`). An incoming link from the steering mechanism (`Steering`) is connected only to the steerable front struts.
- **Body attachment:** the parameter `BodyName = Body` specifies the name of the main physical chassis mesh in the vehicle Blueprint.
- **Coordinate system in `Vehicle.ini`:** right-handed body-local coordinate system in **meters**:
   - **`X` — Forward** (`+X` = front axle, `-X` = rear axle);
   - **`Y` — Left** (`+Y` = left side, `-Y` = right side, opposite to the Unreal Engine 5 `Y` axis);
   - **`Z` — Up**. The strut travel direction is fixed vertically downward along the vector `Direction = 0; 0; -1`.

#### Exact Formula for Converting `Mount` Coordinates from an UE5 Blueprint

In a vehicle Blueprint, the `SuspensionFrontLeft`, `SuspensionFrontRight`, `SuspensionBackLeft`, and `SuspensionBackRight` components are placed at the design wheel centers at normal static ride height, with local coordinates $(X_{\text{UE}},\; Y_{\text{UE}},\; Z_{\text{UE}})$ in **centimeters**.

> [!IMPORTANT]
> **Physical meaning of the `Mount` point and the `Mount.Z` formula:**
> In `Suspension1Spring`, the `Mount` vector defines the wheel center position at **full spring compression** (`SpringSag = 1.0`), and the current wheel center position is measured downward from `Mount` along `Direction = 0; 0; -1`:
>
> $$\vec{P}_{\text{wheel}} = \text{Mount} + (1 - \text{SpringSag}) \cdot \text{Length} \cdot (0;\; 0;\; -1)$$
>
> At normal static sag `NormalSag` (typically `0.33`, i.e., the spring is compressed by $33\%$ of its travel), the wheel sits below `Mount` by the **remaining rebound travel** $(1 - \text{NormalSag}) \cdot \text{Length} = 0.67 \cdot \text{Length}$!
>
> $$\text{Mount} = \left(\frac{X_{\text{UE}}}{100};\quad -\frac{Y_{\text{UE}}}{100};\quad \frac{Z_{\text{UE}}}{100} + (1 - \text{NormalSag}) \cdot \text{Length}\right)$$
>
> *Example:* for `SuspensionFrontLeft` at `(163.0, -85.0, 31.15)` cm with `Length = 0.20` m and `NormalSag = 0.33`, we get $\text{Mount}_Z = 0.3115 + 0.67 \cdot 0.20 = 0.4455\text{ m}$ and write `Mount = 1.63; 0.85; 0.4455`. *(Mistakenly using `+ NormalSag * Length` lifts the body too high by an extra $(1 - 2\cdot\text{NormalSag})\cdot\text{Length} \approx 6.5\text{ cm}$!)*

#### Accounting for the `SpringProgression.csv` Curve and Roll Stiffness

1. **Spring and damper force formulas:** at compression $x = \text{SpringSag} \cdot \text{Length}$ (m) and extension velocity $v$ (m/s, where $v > 0$ is rebound and $v \le 0$ is compression/bump):
   $$F_{\text{spring}} = x \cdot \text{SpringCoeff} \cdot k_{\text{prog}}(\text{SpringSag}) + \text{PreloadForce} \cdot \text{Clamp}\left(\frac{x}{\text{PreloadRampBuffer}},\, 0,\, 1\right), \qquad F_{\text{damp}} = -\frac{v \cdot \text{DampCoeff}}{v > 0 \,?\; 1 : 3}$$
   During compression (bump), `DampCoeff` is automatically divided by `3` (rebound damping is 3 times stiffer than bump damping).
2. **`Common/Vehicles/MotorCars/Tables/SpringProgression.csv` multiplier and choosing `SpringCoeff` / `PreloadForce` / `DampCoeff`:** the standard progression curve (`0 -> 0.55 -> 0.7 -> 0.85 -> 0.95 -> 1.0`), spline-interpolated at `NormalSag = 0.33`, yields a multiplier of **`0.654`** relative to a linear calculation. Given a static corner load $F_{\text{stat}} = m_{\text{corner}} \cdot 9.81$, the spring and damper parameters are related by:
   $$F_{\text{stat}} = \text{PreloadForce} + \text{SpringCoeff} \cdot \text{Length} \cdot \text{NormalSag} \cdot 0.654, \qquad \text{DampCoeff} \approx (0.65 \dots 0.75) \cdot 2 \sqrt{\text{SpringCoeff} \cdot m_{\text{corner}}}$$
3. **No separate anti-roll bar (ARB):** all angular roll stiffness comes solely from the `Suspension1Spring` springs themselves. For tall vehicles (minivans, crossovers/SUVs), it is recommended to choose a natural suspension frequency `10–12%` higher than the purely vertical target and a travel of `Length = 0.20..0.22` m so that the inside wheel retains `~14–15 cm` of droop/rebound travel and does not lift off the road in corners.

| INI Key | Type | Default | Units | Description and Purpose |
| --- | --- | :-: | :-: | --- |
| `BodyName` | string | `"body"` | mesh name | Name of the physical chassis mesh in the Blueprint (`Body`). |
| `Mount` | vector (3) | *required* | m `(X; Y; Z)` | Wheel center coordinates at **full suspension compression** (`SpringSag = 1.0`) in the body coordinate frame. |
| `Length` | number | `0.25` | m | Total suspension travel from full bump stop to full rebound. |
| `SpringCoeff` | number | `50000.0` | N/m | Base spring rate (stiffness). |
| `PreloadForce` | number | `5000.0` | N | Spring preload force. |
| `PreloadRampBuffer` | number | `0.02` | m | Initial compression range `[0..PreloadRampBuffer]` over which `PreloadForce` ramps up smoothly. |
| `DampCoeff` | number | `4000.0` | $\text{N}\cdot\text{s/m}$ | Damper coefficient on **rebound** (automatically divided by `3` on compression/bump). |
| `ProgressionCurve` | CSV path | `"progression.csv"` | 1D table | Spring progression curve (`Common/Vehicles/MotorCars/Tables/SpringProgression.csv`). |
| `NormalSag` | number | `0.33` | fraction `[0..1]` | Nominal static suspension sag of the stationary vehicle (zero point for bump steer). |
| `SagToCamberCurve` | CSV path | `""` | 1D table | Camber-vs-compression curve (`Common/Vehicles/MotorCars/Tables/WheelSagToCamberCurve.csv`). |
| `SagToCamberCoeff` | number | `0.0` | deg | Scale multiplier for the sag-to-camber curve (opposite signs for left and right sides). |
| `SteerToCamberCurve` | CSV path | `""` | 1D table | Camber-vs-steering curve (`Common/Vehicles/MotorCars/Tables/WheelSteerToCamberCurve.csv`). |
| `SteerToCamberCoeff` | number | `0.0` | deg | Scale multiplier for the steer-to-camber curve. |
| `SagToSteeringAngle` | number | `0.0` | deg | Base kinematic bump-steer angle during compression/rebound travel. |
| `SagToSteeringLinCoeff` | number | `0.5` | dimensionless | Linear coefficient of the sag-to-steer polynomial. |
| `SagToSteeringCubeCoeff` | number | `0.5` | dimensionless | Cubic coefficient of the sag-to-steer polynomial. |
| `LonForceToSteeringCoeff` | number | `1.75` | dimensionless | Elastokinematic compliance-steer sensitivity to longitudinal force. |
| `LonForceToSteeringMaxAngle` | number | `0.0` | deg | Maximum compliance-steer angle from longitudinal wheel force (drive/braking). |
| `LatForceToSteeringCoeff` | number | `1.75` | dimensionless | Elastokinematic compliance-steer sensitivity to lateral force. |
| `LatForceToSteeringMaxAngle` | number | `0.0` | deg | Maximum compliance-steer angle from lateral wheel force during cornering. |

- **Outputs (`OutputElements`):** `SpringSag` (`Real`, `[0.0..1.0]` — current relative spring compression), `WheelSteeringAngle` (`Real`, deg — base wheel steering angle).

```ini
[SuspensionFrontLeft]
BodyName = Body
Mount = 1.63; 0.85; 0.4455
Length = 0.20
SpringCoeff = 45000
PreloadForce = 2615
DampCoeff = 4450
NormalSag = 0.33
ProgressionCurve = Common/Vehicles/MotorCars/Tables/SpringProgression.csv
SagToCamberCurve = Common/Vehicles/MotorCars/Tables/WheelSagToCamberCurve.csv
SagToCamberCoeff = -0.3
LonForceToSteeringMaxAngle = 0.5
```

### 1.2. `Suspension1Constant` — Rigid Constant-Load Suspension

A simplified non-raycast suspension with zero compression travel and a fixed vertical load (intended for rail vehicles). Links to a single wheel (`<Link Holder="Suspension..." Reference="Wheel..." />`) and transmits only longitudinal force to the body.

- **`.ini` parameters:** `BodyName` (`"body"`), `Mount` (required vector `X; Y; Z` in m), `Load` (constant vertical load in N, default `100.0`).

### 1.3. `Steering2Ackermann` — Ackermann Steering Linkage

In standard CCD 2.0 schematics, this component is declared as **`[Steering]`** and linked to the two steerable struts (`SuspensionFrontLeft`, `SuspensionFrontRight`).

- The steerable axle track width (`TracksWidth`, $W_{\text{track}}$) is computed automatically from the difference between the lateral `Mount.Y` coordinates of the left and right suspension struts.
- The inside wheel turns by the full angle $\alpha_{\text{inner}} = \text{MaxAngle} \cdot |\text{Steering}|$, while the outside wheel turns by a smaller angle following Ackermann geometry:
   $$\alpha_{\text{outer}} = \arctan\left(\frac{\text{InteraxleDistance}}{W_{\text{track}} + \dfrac{\text{InteraxleDistance}}{\tan(\alpha_{\text{inner}})}}\right)$$

| INI Key | Type | Default | Units | Description |
| --- | --- | :-: | :-: | --- |
| `MaxAngle` | number | `40.0` | degrees | Maximum steering angle of the **inside** wheel at full steering lock (`\|Steering\| = 1.0`). |
| `InteraxleDistance` | number | `1.0` | m | Longitudinal wheelbase of the vehicle — the exact difference $X_{\text{front}} - X_{\text{back}}$ between the front and rear axle `Mount.X` values! |
| `BackSteering` | boolean | `false` | `true`/`false` | If `true`, the axle acts as rear-wheel steering (turns wheels in opposite phase). |
| `MinPositionInput` / `MaxPositionInput` | number | `-1.0` / `1.0` | input units | Input bounds of the `Steering` signal corresponding to full left and full right lock. |

- **Inputs (`InputProcessors`):** `Steering` (`Real`, `[-1.0..+1.0]`, `> 0` = right), `BrokenSteering` (`Boolean` — steering failure).
- **Outputs (`OutputElements`):** `MaxSteeringAngle` (`Real`, rad — average maximum steering angle of the steerable wheels accounting for Ackermann reduction).

### 1.4. `SteeringSimple` — Symmetrical Steering

Turns the wheels of linked suspension struts (`<Link Holder="Steering..." Reference="Suspension..." />`) by an identical angle $\alpha = \text{MaxAngle} \cdot \text{Steering}$ without Ackermann geometry and without `PowerSteering` support.

- **`.ini` parameters:** `MaxAngle` (degrees, default `40.0`). **Inputs:** `Steering` (`Real`, `[-1.0..+1.0]`), `BrokenSteering` (`Boolean`).

---

## 2. Control Input Processors (`Input`)

### 2.1. `FuelControllerComputer` (`[Idler]` section) and `FuelControllerMechanical`

Control throttle opening and per-cycle fuel delivery to the engine (`Engine`).

- **`FuelControllerComputer`** (named **`Idler`** in all standard CCD 2.0 schematics) — ECU for fuel-injected gasoline or diesel ICEs. Requires mandatory `<Link>` connections to `Engine` and `Starter`, plus an optional link to `Speedometer` (for the electronic speed limiter `SpeedLimit`).
- **`FuelControllerMechanical`** — mechanical carburetor with manual choke control (`Choke`). Requires a `<Link>` to `Engine`.

> [!WARNING]
> **Two important simulation quirks of `FuelControllerComputer` (`[Idler]`):**
>
> 1. **Cold-start mixture enrichment (`+10%` torque below `30 °C`):** Over the coolant temperature range `[LowTemperature..WorkingTemperature]` (default `30..35 °C`), the ECU interpolates idle speed from `WarmupRpm` down to `IdleRpm` and holds an enriched air-fuel mixture `Lambda = 0.8`. As a result, a cold engine right after spawn produces roughly **`10%` more torque** than after warming up above `35 °C`.
> 2. **Open-loop idle controller ("anti-stall hump"):** Maintaining `IdleRpm` is computed solely to overcome the engine's own internal friction $F(\text{IdleRpm})$, with no closed-loop feedback for external shaft loads (and no added throttle when RPM drops below `IdleRpm`). When the torque converter impeller loads the crankshaft by `45..60 N·m` in `D` while holding the brake, RPM sags. To keep the engine from stalling and maintain a smooth idle in `D`, a steep "anti-stall torque hump" must be shaped in the `5%` and `10%` rows at the `500 RPM` column of `Engine.csv` (see [Section 2.2 of Document 03](./03_Components_TorqueTrain.md#22-critical-rule-for-calculating-enginecsv-powercurves-and-idling-in-d) and [Section 5.3 of Document 01](./01_Architecture_and_Config.md#53-exact-format-of-the-engine-table-enginecsv-powercurves) for details).

| INI Key | Controller Class | Type | Default | Units | Description |
| --- | --- | --- | :-: | :-: | --- |
| `IdleRpm` | `FuelControllerComputer` | number | `750.0` | RPM | Target idle speed of a fully warmed-up engine. |
| `WarmupRpm` | `FuelControllerComputer` | number | `1000.0` | RPM | Elevated fast-idle warm-up RPM on a cold engine. |
| `LowTemperature` | `FuelControllerComputer` | number | `30.0` | °C | Lower bound of the warm-up temperature range, below which full `WarmupRpm` and `Lambda = 0.8` apply. |
| `WorkingTemperature` | `FuelControllerComputer` | number | `35.0` | °C | Upper bound of coolant warm-up (full transition to operating `IdleRpm` and table `Lambda`). |
| `SpeedLimit` | `FuelControllerComputer` | number | `0.0` | km/h | Top speed limiter (`> 0` when linked to `Speedometer`; smoothly cuts throttle across a $\pm 0.9\text{ km/h}$ band). |
| `Throttle` | `FuelControllerComputer` | array | `0; 0.1; 0.6; 0.7` | fraction `[0..1]` | Accelerator pedal breakpoints for interpolating excess air ratio `Lambda`. |
| `Lambda` | `FuelControllerComputer` | array | `1; 1.07; 1.02; 0.81` | dimensionless | Excess air ratio $\lambda$ at the `Throttle` breakpoints (array lengths must match!). |
| `ThrottleIdle` | `FuelControllerMechanical` | number | `0.15` | fraction `[0..1]` | Minimum carburetor throttle opening at idle. |
| `ThrottleChoke` | `FuelControllerMechanical` | number | `0.30` | fraction `[0..1]` | Throttle opening when the choke is fully pulled (`Choke = 1.0`). |
| `ThrottleIdleMaxRpm` | `FuelControllerMechanical` | number | `1500.0` | RPM | Engine speed above which the `ThrottleIdle` idle stop disengages. |

- **Inputs (`InputProcessors`):** `Accelerator` (`Real`, `[0.0..1.0]`), `FuelFeed` (`Boolean` — supplied by `IgnitionKey`), `Choke` (`Real`, `[0.0..1.0]`, `FuelControllerMechanical` only).

### 2.2. `AbsTcs` — ABS, TCS, and Electronic Brakeforce Distribution (EBD) Unit

In standard CCD 2.0 schematics, this component is declared as **`[AbsTcs]`** strictly **before** `Idler`, powered from the ignition circuit (`IgnitionElectricCircuit`), and linked to `Idler` and all four brakes (`BrakeFrontLeft`, `BrakeFrontRight`, `BrakeBackLeft`, `BrakeBackRight`).

- **Electronic Brakeforce Distribution (EBD):** automatically proportions requested braking torque across axles according to each wheel's current vertical load (with a `5%` floor of nominal load for unloaded/lifted wheels).
- **ABS and TCS:** when bus voltage exceeds `MinVoltage`, keep each wheel's longitudinal slip within the optimal window (`OptimalSlip` / `MaxSlip` / `SlipRange` from the tire's longitudinal friction curve) and forward the `Accelerator` signal to `Idler`.

| INI Key | Type | Default | Units | Description |
| --- | --- | :-: | :-: | --- |
| `AbsEquipped` / `TcsEquipped` | boolean | `true` / `true` | `true`/`false` | Whether the vehicle is equipped with ABS and TCS (ASR). |
| `AbsIsOnByDefault` | boolean | `true` | `true`/`false` | **Note:** this single key enables **both ABS and TCS simultaneously** by default when power is applied! |
| `Resistance` | number | `5.0` | Ω | Electrical resistance of the unit in the ignition circuit. |
| `MinVoltage` / `NormalVoltage` | number | `9.0` / `12.0` | V | Minimum and nominal bus voltages required for ABS and TCS operation. |

- **Inputs (`InputProcessors`):** `Brake` (`Real`, `[0..1]`), `Accelerator` (`Real`, `[0..1]`), `EnableABS` (`Boolean`), `EnableTCS` (`Boolean`), `Enabled` (`Boolean`).
- **Outputs (`OutputElements`):** `AbsActive` (`Boolean`), `TcsActive` (`Boolean`), `AbsEnabled` (`Boolean`), `TcsEnabled` (`Boolean`), `Enabled` (`Boolean`), `Intensity` (`Real`).

### 2.3. `ClutchInputProcessor` — Nonlinear Clutch Pedal Controller

In manual-transmission schematics (`*Manual.xml`), declared under the name **`[ClutchPedal]`** (`<Link Holder="ClutchPedal" Reference="Clutch" />`).

- **`Clutch` input inversion:** `0.0` = pedal **fully pressed** (clutch disengaged), `1.0` = pedal **fully released** (clutch engaged). Disc engagement law: $\text{Position} = (\text{Clutch})^{\text{Power}}$ (with `Power = 4.0`, the smooth bite zone lies around `0.70..0.85`).
- **`.ini` parameters:** `Power` (number, default `4.0`).

### 2.4. `GearBoxAutomaticLogic` (`[GearBoxLogic]` section) and `GearBoxAutomaticLogicBus`

Automatic transmission selector controllers (`<Link Holder="GearBoxLogic" Reference="GearBoxAutomatic" />`) managing selector modes `0 = P`, `1 = R`, `2 = N`, `3 = D` and the upper gear limit `AutoShifterMaxGear` (`MaxGear`).

- **`GearBoxAutomaticLogic`** (passenger-car selector in `*Auto.xml`): interlocks shifting out of Park `P (0)` unless the brake pedal is depressed (`Brake >= ShiftMinBrake`), and when `ConsiderRpmWhenChangeState = true`, protects the transmission from engaging `P`, `R`, or `D` above the output shaft speed thresholds (`MaxParkingOutputRpm`, `MaxReverseOutputRpm`, `MaxDriveOutputRpm`), temporarily holding neutral `N (2)`.
- **`GearBoxAutomaticLogicBus`** (push-button bus selector `D / N / R`): forbids direct `D <-> R` transitions bypassing `N`, supports a reverse interlock `ReverseLock`, and automatically engages a hidden neutral when stopped with the brake pressed (`Brake > 0.1`).

| INI Key | Class | Type | Default | Units | Description |
| --- | --- | --- | :-: | :-: | --- |
| `HasParkingState` | Both | boolean | `true` | `true`/`false` | Whether the Park mode `P` (`0`) is present. Set to `false` for buses without Park. |
| `ShiftMinBrake` | `GearBoxAutomaticLogic` | number | `0.1` | fraction `[0..1]` | Minimum brake pedal depression required to shift out of `P`. |
| `ConsiderRpmWhenChangeState` | `GearBoxAutomaticLogic` | boolean | `true` | `true`/`false` | Enables protection against shifting into `P`, `R`, or `D` while the gearbox output shaft is spinning rapidly. |
| `MaxParkingOutputRpm` | `GearBoxAutomaticLogic` | number | `50.0` | RPM | Maximum transmission output shaft RPM at which engaging `P` is allowed. |
| `MaxReverseOutputRpm` | `GearBoxAutomaticLogic` | number | `50.0` | RPM | Maximum forward-rolling output shaft RPM at which engaging `R` is allowed. |
| `MaxDriveOutputRpm` | `GearBoxAutomaticLogic` | number | `50.0` | RPM | Maximum backward-rolling output shaft RPM at which engaging `D` is allowed. |
| `HasReverseLock` | `GearBoxAutomaticLogicBus` | boolean | `false` | `true`/`false` | Whether an unlock signal (`ReverseLock = false`) is required to engage reverse `R`. |

- **Inputs:** `AutoShifterState` (`Integer`: `0=P`, `1=R`, `2=N`, `3=D`), `AutoShifterMaxGear` (`Integer`), `Brake` (`Real`), `Accelerator` (`Real`), `ReverseLock` (`Boolean`). **State:** `AutoShifterState` (`InternalName="State"`, `Integer`).
- **Outputs:** `AutoShifterState`, `CurrentState`, `AutoShifterMaxGear`, `AutoShifterCurrentGear`, `AutoShifterTargetGear` (all `Integer`).

### 2.5. `IgnitionKeyProcessor` — Ignition Switch Controller

Declared in standard schematics as **`[IgnitionKey]`** (has no parameters in `Vehicle.ini`) and accepts the **`Ignition`** input (`Integer`: `0 = Off`, `1 = Ignition`, `2 = Starter`).

- **`<Link Holder="IgnitionKey" Reference="..." />` connections:**
   1. `IgnitionElectricCircuit` — closes the ignition circuit whenever `Ignition != 0`.
   2. `Starter` and `StarterRelay` — energizes the starter motor and its relay when `Ignition == 2`.
   3. `Idler` (`FuelControllerComputer`) — sends `FuelFeed = (Ignition != 0)`. **Critical for diesel engines (`EngineDiesel`):** this exact link cuts fuel supply and shuts off a diesel engine when the key is turned to `0`!
   4. `GearBoxLogic` (`GearBoxAutomaticLogic`) — **in-gear starter interlock**: if the AT selector is in `D (3)` or `R (1)`, key position `2 (Starter)` is forced down to `1 (Ignition)`.

---

## 3. Telemetry and Steering Force Feedback (`Output`)

### 3.1. `PowerSteering` — Power Steering (HPS/EPS) and Force Feedback

Declared in standard schematics as **`[PowerSteering]`** (`<Link>` to `Steering` and `Engine`). Computes steering-shaft Force Feedback (FFB) from tire self-aligning torque and lateral forces, reducing steering effort as engine RPM rises from `ActivationRpm` to `FullPowerRpm`.

| INI Key | Type | Default | Units | Description |
| --- | --- | :-: | :-: | --- |
| `SteeringRatio` | number | `17.0` | dimensionless | Steering gear ratio (ratio of steering wheel angle to road wheel angle). |
| `ActivationRpm` / `FullPowerRpm` | number | `0.0` / `0.1` | RPM | Engine RPM at which power assist starts and reaches full output (typically `FullPowerRpm = 500`). |
| `MaxTorque` | number | `1.0` | $\text{N}\cdot\text{m}$ | Maximum assist torque on the steering shaft (typically `35..45`). |
| `TorqueThreshold` | number | `1.0` | $\text{N}\cdot\text{m}$ | Steering torque threshold below which assist does not cancel feedback (preserves on-center feel, `1.5..2.5`). |
| `ConstFriction` | number | `0.0` | $\text{N}\cdot\text{m}$ | Constant dry friction torque in the steering column. |
| `DampingCoeff` | number | `0.0` | $\text{N}\cdot\text{m}\cdot\text{s/rad}$ | Viscous damping coefficient of steering wheel rotation for FFB. |
| `MechanicalTrailCoeff` | number | `0.0` | m | Mechanical caster trail arm: adds restoring torque $-F_{\text{lat}} \cdot \text{MechanicalTrailCoeff}$ (response to slip angle and lateral bumps, `0.002..0.025`). |

- **Inputs:** `BrokenPowerSteering` (`InternalName="Broken"`, `Boolean`).
- **Outputs:** `ForceFeedbackTorque` (`InternalName="Torque"`), `ForceFeedbackFriction` (`InternalName="Friction"`), `ForceFeedbackDamping` (`InternalName="Damping"`), `ForceFeedbackSlipAngle` (`InternalName="SlipAngle"`), `ForceFeedbackInertia` (`InternalName="Inertia"`).

### 3.2. `Speedometer` — Speedometer and Odometer

Declared in standard schematics as **`[Speedometer]`** and linked to the primary differential (`DifferentialAxle` in `Awd*` schematics or `Differential` in `Fwd*`/`Rwd*` schematics).

- **Exact `MetersPerRotation` formula:** distance traveled per driveshaft revolution equals the wheel circumference **divided** by the final drive ratio `GearRatio` of the differential to which the speedometer is linked:
   $$\text{MetersPerRotation} = \frac{2\pi R_{\text{wheel}}}{i_{\text{diff}}}$$
   *(When MT and AT have different final drive ratios, specify this separately in `[Speedometer.TransmissionType.Manual]` and `[Speedometer.TransmissionType.Auto]`.)*
- **Inputs:** `MileageMultiplier` (`Real`, `1.0`). **States:** `Mileage` (m), `SessionMileage` (m). **Outputs:** `Speed` (m/s), `Mileage` (m), `SessionMileage` (m).

### 3.3. `FuelConsumptionMeter` — Average Fuel Consumption Meter

Trip computer tracking rolling and session average fuel consumption (`<Link>` to `Speedometer` and `Engine`).

- **`.ini` parameters:** `IsEquipped` (`Boolean`, `false`), `AveragingDistance` (km, `1.0`), `AveragingStep` (m, `100.0`).
- **States:** `AverageFuelConsumption` (L/m), `SessionMileage` (m), `SessionFuelConsumed` (L), `IsEnabled` (`Boolean`). **Outputs:** `AverageFuelConsumption` (L/m), `SessionAverageFuelConsumption` (L/m; multiply by `100000` to convert to L/100 km).

---

## 4. Forced Induction, Pneumatics, Tanks, Aerodynamics, and External Forces

### 4.1. `Turbocharger` — Engine Forced-Induction Turbocharger

Increases intake manifold air pressure (`<Link Holder="Turbocharger" Reference="Engine" />`, input `<Processor Input="Accelerator" Component="Turbocharger" InternalName="Throttle" />`), proportionally multiplying indicated combustion torque.

| INI Key | Type | Default | Units | Description |
| --- | --- | :-: | :-: | --- |
| `MinRpm` | number | `1500.0` | RPM | Minimum effective speed ($\text{RPM} \cdot \text{Throttle}$) at which boost begins ($\text{ratio} > 1.0$). |
| `RefRpm` | number | `6000.0` | RPM | Reference speed (`> MinRpm`) at which `RefCompressionRatio` is reached under full throttle. |
| `RefCompressionRatio` | number | `1.6` | dimensionless | Intake pressure compression ratio at `RefRpm` with `Throttle = 1.0`. |
| `LagTime` | number | `1.0` | s | Turbocharger spool inertia parameter (**`0.08..0.10`** recommended — see Rule #1 below!). |

> [!CAUTION]
> **Two critical rules for calculating `Turbocharger` and `Engine.csv`:**
>
> 1. **Fourfold spool-time multiplier ($\tau_{\text{eff}} = 4 \cdot \text{LagTime}$):** effective turbine speed is smoothed at a rate of $\frac{0.25}{\text{LagTime}} \cdot dt$, so the true time constant is **$\tau_{\text{eff}} = 4 \cdot \text{LagTime}$**, and at idle (`Throttle = 0`) turbine speed drops to `0`. To avoid a 1.5-second boost dead zone off the line and achieve a realistic `0.32..0.40 s` response, set **`LagTime = 0.08..0.10`**.
> 2. **Unbounded growth above `RefRpm` and 2D division of `Engine.csv` by `(RPM, Throttle)`:** the boost ratio formula:
>    $$\text{ratio}(\text{RPM}, \text{Throttle}) = 1 + (\text{RefCompressionRatio} - 1)\sqrt{\frac{\max(0,\; \text{RPM} \cdot \text{Throttle} - \text{MinRpm})}{\text{RefRpm} - \text{MinRpm}}}$$
>    has **no upper cap** above `RefRpm` and depends on `Throttle`! Every combustion cell `(thr, RPM)` in `Engine.csv` must be divided by **its own** $\text{ratio}(\text{RPM}, \text{thr})$ at that row's `thr = row_pct / 100.0`, while leaving the final mechanical-friction row $F(\text{RPM})$ untouched.

### 4.2. Pneumatic System: `Receiver`, `ValveMultiCircuit`, `ActuatorPneumatic1Chamber`, `ActuatorPneumatic2Chamber`

Components for air brakes and pneumatic bus/truck door actuators (pressures are specified in **absolute bar**, where atmospheric pressure is $\approx 1.0\text{ bar}$):

- **`Receiver` (air tank)** and **`ValveMultiCircuit` (multi-circuit protection valve):**
   The declaration order of `<Link Holder="ValveMultiCircuit" Reference="Receiver..." />` links in the XML schematic is **critical** — it defines the circuit index `i`, which maps directly to `OpenPressure[i]` and `ClosePressure[i]` in `Vehicle.ini`!
- **`ActuatorPneumatic1Chamber` (single-chamber actuator)** and **`ActuatorPneumatic2Chamber` (dual-chamber door cylinder):**
   Linked to their supply air receiver (`<Link Holder="Actuator..." Reference="Receiver..." />`) and accept the `Position` input (`Real`, `[0.0..1.0]`).

| INI Key | Components | Default | Units | Description |
| --- | --- | :-: | :-: | --- |
| `Volume` | `Receiver`, `ValveMultiCircuit` | `1.0` | L | Geometric volume of the air tank / valve chamber. |
| `MaxPressure` / `WorkPressure` | All 4 components | `8.0` / `8.0` (`6.0`) | bar (abs.) | Opening (`MaxPressure - 0.05`) and closing (`WorkPressure + 0.05`) pressures of the blow-off relief valve. |
| `BlowOffDiameter` / `InitialPressure` | All 4 components | `5.0` / atm. | mm / bar | Safety blow-off valve orifice diameter and initial absolute air pressure. |
| `PressureDelta` | `ValveMultiCircuit`, `Actuator*` | `1.0` | bar | Pressure differential for smooth opening of the transfer valve. |
| `ValveDiameter` / `AirLossDiameter` | `ValveMultiCircuit` | `10.0` / `1.0` | mm | Per-circuit valve orifice diameter and natural inlet air-leakage orifice diameter. |
| `OpenPressure` / `ClosePressure` | `ValveMultiCircuit` | *empty* | bar | Arrays of forward-charging opening and backflow closing pressure thresholds per circuit (`P1; P2; ...`). |
| `MinVolume` / `MaxVolume` | `ActuatorPneumatic1/2Chamber` | `1.0` / `2.0` | L | Minimum (compressed) and maximum (expanded) working chamber volumes. |
| `MinVolumePressure` / `MaxVolumePressure` | `ActuatorPneumatic1/2Chamber` | `4.0` / `7.0` | bar (abs.) | Target chamber pressures at `Position = 0.0` and `Position = 1.0`. |
| `FeedDiameter` / `LeakageDiameter` | `ActuatorPneumatic1/2Chamber` | `10.0` / `5.0` | mm | Charging transfer valve diameter and emergency leakage orifice diameter. |
| `PistonMass` | `ActuatorPneumatic2Chamber` | `0.0` (**`> 0`!**) | kg | Mass of the piston and door leaf. **Must be set `> 0`, otherwise division by zero occurs!** |
| `PistonFriction` / `PistonDiameter` | `ActuatorPneumatic2Chamber` | `0.0` / `1.0` | N / m | Static breakaway friction force and piston diameter (`> 0`). |
| `PistonInitialPosition` | `ActuatorPneumatic2Chamber` | `0.5` | fraction `[0..1]` | Initial piston position (`0.0` = far left, `1.0` = far right). |

- **Outputs:** `Receiver` — `Pressure` (bar); `ActuatorPneumatic1Chamber` — `WorkPressure`, `MaxVolumePressure`; `ActuatorPneumatic2Chamber` — `WorkPressure`, `MaxVolumePressure`, `Position` (`[0..1]`), `PressureLeft`, `PressureRight`.

### 4.3. `Tank` — Fuel and Oil Tank

Used for the fuel tank **`[FuelTank]`** and automatic transmission oil tank **`[OilTank]`** (in `*Auto.xml` schematics).

- **`.ini` parameters:** `Volume` (capacity in L, default `1.0`, must be `> 0`), `StartLevel` (initial fill fraction `[0.0..1.0]`, default `1.0`).
- **Inputs:** `FuelLeak` (`InternalName="DrainSpeed"`, L/s), `UnlimitedFuelTank` (`InternalName="EnableUnlimited"`, `Boolean`). **State:** `FuelVolume` (`InternalName="LiquidVolume"`, L). **Outputs:** `FuelVolume` (`LiquidVolume`, L), `FuelVolumeMax` (`Volume`, L), `FuelRelative` (`LiquidRelativeVolume`, `[0..1]`).

### 4.4. `AerodynamicsCarSimple` — Body Aerodynamics

Declared in standard schematics as **`[Aerodynamics]`**. Applies aerodynamic drag $F_{\text{drag}} = \frac{1}{2}\rho_{\text{air}} C_d A v^2$ ($\rho_{\text{air}} = 1.225\text{ kg/m}^3$) and vertical lift/downforce $F_{\text{lift}} = \frac{1}{2}\rho_{\text{air}} C_l A v_{\text{fwd}}^2$ at the body's center of mass (`BodyName = Body`).

- **`.ini` parameters:** `BodyName` (`Body`), `DragCoeff` ($C_d$, default `0.4`), `LiftCoeff` ($C_l$, default `-0.3`; `< 0` = downforce), `FrontalArea` (frontal cross-sectional area in $\text{m}^2$, default `2.0`).

### 4.5. `ExternalThruster` — External Linear Thruster

Applies a controllable force at body-local point `Pivot` along vector `Direction`, with thrust tapering linearly to zero between speeds `MaxForceVelocity` and `ZeroForceVelocity`.

- **`.ini` parameters:** `BodyName` (`"body"`), `Pivot` (required vector `X; Y; Z` in m), `Direction` (required vector `X; Y; Z`), `MaxForce` (N, `100.0`), `MaxForceVelocity` (m/s), `ZeroForceVelocity` (m/s), `MinPower` (`-1.0`), `MaxPower` (`1.0`). **Input:** `Power` (`Real` or `Boolean`).
