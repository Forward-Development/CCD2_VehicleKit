# 04. Onboard Electrical System and Dashboard (`CarElectrics`) in City Car Driving 2.0

**Navigation:** [README](./README.md) | [00. Quick Start](./00_Quick_Start.md) | [01. Architecture & Configs](./01_Architecture_and_Config.md) | [02. Named I/O & States](./02_Named_IO_and_States.md) | [03. TorqueTrain](./03_Components_TorqueTrain.md) | **04. Electrics** | [05. Chassis & Subsystems](./05_Components_Chassis_and_Subsystems.md)

## Table of Contents

1. [Vehicle Electrical Network Schematic in City Car Driving 2.0](#1-vehicle-electrical-network-schematic-in-city-car-driving-20)
2. [Components `ElectricCircuit`, `Battery`, and `ElectricElement`](#2-components-electriccircuit-battery-and-electricelement)
3. [Onboard Electrics, Lighting, and Dashboard `CarElectrics`](#3-onboard-electrics-lighting-and-dashboard-carelectrics)
4. [Ready-to-Use `[CarElectrics]` and `[Battery]` Example from CCD 2.0](#4-ready-to-use-carelectrics-and-battery-example-from-ccd-20)

---

## 1. Vehicle Electrical Network Schematic in City Car Driving 2.0

In all standard XML schematics (`Common/Vehicles/MotorCars/Schemes/*.xml`), the onboard DC electrical network is built from three nodal circuits (`Class="ElectricCircuit"`):

1. **`BatteryElectricCircuit`** — the always-live power circuit of the battery (`Battery`) and starter (`Starter`). It powers devices that operate even with the ignition key removed (hazard lights `HazardLight`, parking lights `ParkingLights`, brake lights `StopLight`, and horn `Horn`).
2. **`GeneratorElectricCircuit`** — the alternator circuit (`Generator`), permanently connected to `BatteryElectricCircuit`.
3. **`IgnitionElectricCircuit`** — the switched ignition circuit (Terminal 15). In XML, it is connected to `BatteryElectricCircuit` with the attribute **`Connected="false"`**:

```xml
<Link Holder="BatteryElectricCircuit" Reference="Battery" />
<Link Holder="BatteryElectricCircuit" Reference="Starter" />
<Link Holder="BatteryElectricCircuit" Reference="CarElectrics" />
<Link Holder="BatteryElectricCircuit" Reference="GeneratorElectricCircuit" />
<Link Holder="GeneratorElectricCircuit" Reference="Generator" />
<Link Holder="BatteryElectricCircuit" Reference="IgnitionElectricCircuit" Connected="false" />
<Link Holder="IgnitionElectricCircuit" Reference="Engine" /> <!-- EngineGazoline only! -->
<Link Holder="IgnitionElectricCircuit" Reference="CarElectrics" />
<Link Holder="IgnitionKey" Reference="IgnitionElectricCircuit" />
```

When the driver turns the ignition key (`IgnitionKey`, `Class="IgnitionKeyProcessor"`) to position `1` (`Ignition`) or `2` (`Starter`), `IgnitionKey` closes the `BatteryElectricCircuit -> IgnitionElectricCircuit` connection, supplying onboard bus voltage to ignition-switched equipment, headlights, turn signals, wipers, the dashboard, and the spark plugs of a gasoline ICE.

> [!IMPORTANT]
> For devices in `[CarElectrics]` to receive power from a specific circuit (for example, `IgnitionElectricCircuit` or `BatteryElectricCircuit`), a `<Link Holder="CircuitName" Reference="CarElectrics" />` link must be declared in `<Links>`, and that circuit's name must be specified in `Vehicle.ini` via `DefaultCircuitName` or `<Prefix>_CircuitName`.

---

## 2. Components `ElectricCircuit`, `Battery`, and `ElectricElement`

### 2.1. Electrical Circuit (`Class="ElectricCircuit"`)

Computes the common bus voltage of parallel-connected EMF sources (`Battery`, `Generator`) and consumers according to Ohm's law, accounting for their internal resistances. Has no parameters in `Vehicle.ini`.

- **`<Links>` connections (`Holder="CircuitName"`):** accepts `Battery`, `Generator`, `Starter`, `Engine` (`EngineGazoline`), `CarElectrics`, standalone `ElectricElement` components, and child `ElectricCircuit` circuits (with the optional attribute `Connected="true|false"`).
- **Input (`InternalName`):** `Master` (`Boolean`) — closes (`true`) or opens (`false`) the connection between this circuit and its parent circuit. **Output (`InternalName`):** `Voltage` (`Real`, V) — current bus voltage in the circuit.

### 2.2. Battery (`[Battery]`, `Class="Battery"`)

A DC power source that discharges under load (especially during starter cranking) and charges whenever the alternator bus voltage exceeds the battery's current EMF.

| Key in `Vehicle.ini` (`[Battery]`) | Type | Default | Units | Description |
| --- | --- | :-: | :-: | --- |
| `VoltageMin` | `Real` | `10.0` | V | Minimum EMF when the battery is completely discharged (`10.0–10.5`). |
| `VoltageMax` | `Real` | `12.0` | V | Maximum EMF of a fully charged battery (`12.6–14.7`). |
| `Capacity` | `Real` | `60.0` | Ah | Battery capacity in ampere-hours (`55–100`). |
| `Resistance` | `Real` | `0.02` | Ω | Base internal resistance of the battery (`0.002–0.02`). |
| `DischargeResistance` | `Real` | `Resistance` | Ω | Internal resistance while delivering current (discharging). |
| `ChargeResistance` | `Real` | `Resistance` | Ω | Internal resistance while accepting current (charging from the alternator). |

- **Inputs (`InternalName`):** `Battery` (`Boolean`, default `true` — battery ground/master disconnect switch), `EnableUnlimited` (`Boolean` — infinite charge mode `UnlimitedBattery`).
- **Outputs (`InternalName`):** `Amperage` (`Real`, A: `> 0` — discharging, `< 0` — charging), `VoltageMin` (`Real`, V), `CapacityRelative` (`Real`, `[0..1]`), `BatteryActive` (`Boolean` — `true` when the battery is discharging).

### 2.3. Standalone Electrical Consumer (`Class="ElectricElement"`)

A general-purpose resistive electrical device (heater, auxiliary light, emergency beacon) connected to an `ElectricCircuit`.

- **`.ini` parameters:** `Resistance` (`5.0` Ω — resistance when turned on), `MinVoltage` (`9.0` V — turn-on voltage threshold), `NormalVoltage` (`12.0` V — voltage for 100% brightness/power).
- **Input (`InternalName`):** `Enabled` (`Boolean`). **Outputs (`InternalName`):** `Intensity` (`Real`, `[0..∞)`), `Enabled` (`Boolean`).

---

## 3. Onboard Electrics, Lighting, and Dashboard `CarElectrics`

The `<Component Name="CarElectrics" Class="CarElectrics" />` component (section **`[CarElectrics]`** in `Vehicle.ini`) consolidates all exterior/interior lighting, turn signals, windshield wipers, horn, warning buzzers, telltale warning icons, and analog dashboard gauges.

### 3.1. `<Links>` Connections to `CarElectrics` in XML

So that `CarElectrics` automatically reads component telemetry for dashboard needles and telltale lamps, `<Link Holder="CarElectrics" Reference="ComponentName" />` entries are declared in `<Links>`:

| Target Component (`Reference`) | XML Attribute in `<Link>` | Gauges and Lamps in `CarElectrics` Driven by This Link |
| --- | :-: | --- |
| `Engine` (`EngineGazoline` / `EngineDiesel`) | — | Tachometer `RpmNeedle`, coolant temperature gauge `CoolantTemperatureNeedle`, oil pressure gauge `OilNeedle`, icons `OilIcon`, `HighOilPressureIcon`, `OverheatIcon`, `PowerSteeringIcon`, `ParkingBrakeIcon`, `EmergencyStopIcon`, `BrakeOilLowLevelIcon`, buzzer `Beeper`. |
| `Speedometer` | — | Speedometer `SpeedNeedle`, unfastened seatbelt warning icon and buzzer `SafetyBeltIcon`, `SafetyBeltBeeper`. |
| `FuelTank` (`Class="Tank"`) | — | Fuel level gauge `FuelNeedle` (in liters) and low-fuel warning lamp `FuelIcon`. |
| `Generator` | — | No-charge battery warning lamp `BatteryIcon` (turns off once the alternator reaches operating output). |
| `Battery` | — | Ammeter gauge `AmperageNeedle`. |
| `IgnitionKey` (`IgnitionKeyProcessor`) | — | Glow plug / pre-heater indicator lamp `HeaterControlIcon`. |
| `StarterRelay` (`Class="Relay"`) | — | Starter cranking monitor lamp `StartControlIcon`. |
| `GearBox` (in `*Manual.xml`) | — | Reverse light `ReverseLight` (illuminates when gear ratio `< 0`). |
| `GearBox` | `StartIconGearBox="true"` | Transmission neutral check for the blinking mode of `StartControlIcon`. |
| `GearBox` | `Reducer="true"` | Transfer case / range splitter for `DownshiftAWDIcon` and `OverdriveAWDIcon`. |
| `Relay` (axle couplings) | `AxisRelay="true"` | Driven-axle engagement icons `DriveAxis0..2Icon`, `DownshiftAWDIcon`, `OverdriveAWDIcon` *(also require a link with `Reducer="true"`)*. |
| `GearBoxLogic` (in `*Auto.xml`) | — | Reverse light `ReverseLight` and AT selector mode indicators `GearBoxIconN`, `GearBoxIconR`, `GearBoxIconD`, `GearBoxIcon1`, `GearBoxIcon2`. |
| `GearBoxAutomaticPump` (`Class="Pump"`) | — | Transmission oil pressure gauge and warning lamps: `GearBoxOilNeedle`, `GearBoxOilIcon`, `GearBoxOilHighPressureIcon`. |
| `AbsTcs` | — | Warning/activity icons `AbsDisableIcon`, `AbsActiveIcon`, `TcsDisableIcon`, `TcsActiveIcon`. |
| `BrakeFrontLeft/Right`, `BrakeBackLeft/Right` | — | Parking brake light `ParkingBrakeLight` (when handbrake is applied `> 0.01`). |
| `WheelFrontLeft/Right`, `WheelBackLeft/Right` | — | Low tire pressure warning lamp (TPMS) `LowTirePressureIcon`. |
| Door actuators (`ActuatorPneumatic2Chamber`) | — | Open bus/body door warning lamps `DoorIcon0` .. `DoorIcon3`. |

### 3.2. General Parameters of `[CarElectrics]` in `Vehicle.ini` (17 Parameters)

| Key in `Vehicle.ini` (`[CarElectrics]`) | Type | Default | Units | Description |
| --- | --- | :-: | :-: | --- |
| `DefaultCircuitName` | `String` | `""` | circuit name | Default electrical circuit name for all elements (usually `IgnitionElectricCircuit`). |
| `MinVoltage` | `Real` | `9.0` | V | Minimum bus voltage below which lamp brightness is `0` (`0 < MinVoltage <= Voltage`). |
| `Voltage` | `Real` | `13.0` | V | Nominal bus voltage at which lamp brightness equals `1.0` (`13.0–14.7`). |
| `HasAlwaysLitDashboard` | `Boolean` | `false` | — | If `true`, the `AlwaysLitDashboard` instrument cluster backlight stays illuminated whenever ignition is on (regardless of parking lights). |
| `LowFuelLevel` | `Real` | `0.1` | `[0..1]` | Remaining fuel fraction below which the low-fuel warning lamp `FuelIcon` illuminates (e.g., `0.15–0.21`). |
| `OilLowPressureThreshold` | `Real` | `0.6` | bar | Critically low engine oil pressure threshold that triggers `OilIcon`. |
| `OilHighPressureThreshold` | `Real` | `6.1` | bar | Excessively high engine oil pressure threshold for `HighOilPressureIcon`. |
| `OilPressureConsiderEngine` | `Boolean` | `false` | — | If `true`, engine oil pressure lamps light up only while the engine is running; if `false`, they also illuminate with ignition on before engine start. |
| `GearBoxOilLowPressureThreshold` | `Real` | `1.0` | bar | Low transmission oil pressure threshold for `GearBoxOilIcon`. |
| `GearBoxOilHighPressureThreshold` | `Real` | `10.0` | bar | High transmission oil pressure threshold for `GearBoxOilHighPressureIcon`. |
| `GearBoxOilPressureConsiderEngine` | `Boolean` | `false` | — | Whether to consider engine running state for transmission oil pressure warning lamps. |
| `HasBrokenBrakeBeeper` | `Boolean` | `true` | — | Whether to activate the warning buzzer `Beeper` upon brake failure (`BrokenBrake = true`) while the engine is running. |
| `HasEngineOverheatBeeper` | `Boolean` | `true` | — | Whether to activate the warning buzzer `Beeper` when the engine overheats (`CoolantTemperature > OverheatTemperature`). |
| `HeaterControlIcon_BlinkingTime` | `Real` | `2.0` | s | Duration of steady illumination of the glow-plug icon `HeaterControlIcon` before switching to blinking mode. |
| `SafetyBeltMinSpeed` | `Real` | `2.0` | km/h | Minimum driving speed above which the unfastened seatbelt timer starts counting. |
| `SafetyBeltWarningSpeed` | `Real` | `20.0` | km/h | Speed threshold above which driving unbuckled immediately triggers `SafetyBeltBeeper` and blinks `SafetyBeltIcon`. |
| `SafetyBeltWarningTime` | `Real` | `60.0` | s | Duration of driving faster than `SafetyBeltMinSpeed` unbuckled before `SafetyBeltBeeper` sounds. |

### 3.3. Configuring Lamps, Headlights, Turn Signals, Wipers, and Gauges via `<Prefix>_*`

Inside `[CarElectrics]`, each of the 81 devices is configured using keys of the form `<ElementPrefix>_<Suffix>`:

| Element Category | Parameter Suffix in `[CarElectrics]` | Type | Default | Units | Description |
| --- | --- | --- | :-: | :-: | --- |
| **All 81 elements** (headlights, lamps, icons, buzzers, wipers, needles) | `<Prefix>_Resistance` | `Real` | **`0.0`** | Ω | Electrical resistance of the active device (`1.2–7.0` for exterior lights, `40` for dashboard icons, `100` for needles). |
|  | `<Prefix>_CircuitName` | `String` | `DefaultCircuitName` | circuit name | Per-device circuit assignment (e.g., `BatteryElectricCircuit` for `HazardLight`, `ParkingLights`, `StopLight`, `Horn`). |
|  | `<Prefix>_ConnectionCheckTime` | `Real` | `0.0` *(or $\infty$)* | s | Lamp bulb-check duration after power is applied to the circuit (dashboard self-test on key turn). |
|  | `<Prefix>_EngineCheckTime` | `Real` | `0.0` | s | Lamp test duration **after engine start** before extinguishing. |
| **Blinking elements** (`*TurnLight*`, `HazardLight*`, `SafetyBeltIcon`, `ParkingBrakeIcon`, `AbsActiveIcon`, `TcsActiveIcon`, `StartControlIcon`, `HeaterControlIcon`, `EngineShutdownIcon`) | `<Prefix>_Period` | `Real` | `0.6` (`0.0` for ABS/TCS) | s | Full blinking cycle period (`1.0` = 60 flashes/min; `0.0` lights continuously without blinking). |
|  | `<Prefix>_DutyCycle` | `Real` | `0.5` | `[0..1]` | Fraction of the period during which the lamp is lit (`0.5` = half-period on, half-period off). |
|  | `<Prefix>_Blinking` | `Boolean` | `true` | — | Whether blinking is enabled for this element by default. |
| **Windshield wipers** (`FrontWindshieldWiper`, `BackWindshieldWiper`) | `<Prefix>_PeriodicDelay` | `Real` | `2.0` | s | Pause between blade sweeps in intermittent mode (`1`). |
|  | `<Prefix>_PeriodicPhase` | `Real` | `2.0` | s | Full back-and-forth sweep time (`0 -> 1 -> 0`) in intermittent mode (`1`). |
|  | `<Prefix>_NormalPhase` | `Real` | `2.0` | s | Full back-and-forth sweep time in continuous mode (`2`) and when returning to park (`0`). |
|  | `<Prefix>_FastPhase` | `Real` | `1.0` | s | Full back-and-forth sweep time in fast mode (`3`). |
| **Analog gauges** (`*Needle`) | `<Prefix>_IsMechanical` | `Boolean` | `false` | — | If `true`, the gauge is mechanical — always operates without electrical power and does not require `_Resistance`. |
|  | `<Prefix>_DefaultValue` | `Real` | `0.0` | scale units | Needle resting reading when ignition is off (for example, `50` for `CoolantTemperatureNeedle`). |
|  | `<Prefix>_ValueSpeed` | `Real` | `5.0` | $1/\text{s}$ | Smoothing speed at which the needle tracks its target value. |

> [!IMPORTANT]
> **Two golden rules for configuring `[CarElectrics]` elements:**
>
> 1. **Zero resistance = device disabled!** By default, all 81 elements have `<Prefix>_Resistance = 0.0`, which treats them as disconnected from the electrical network so their output to the game is always **`0.0`**. For any headlight, warning lamp, buzzer, wiper, or electrical needle to function, **you must assign a positive resistance `<Prefix>_Resistance > 0` in `[CarElectrics]`** (the only exception is mechanical gauges with `<Prefix>_IsMechanical = true`).
> 2. **`<Prefix>_EngineCheckTime` quirk:** if you set `_EngineCheckTime > 0` for a lamp (e.g., `ExhaustSystemIcon_EngineCheckTime = 0.5`) without setting `_ConnectionCheckTime`, then `_ConnectionCheckTime` automatically becomes **infinite** — the lamp stays lit the entire time ignition is on and turns off `0.5 s` after the engine starts!

### 3.4. Complete Reference of All 81 `[CarElectrics]` Elements and Their XML Outputs

The "Type" column indicates the supported suffix set: **Basic** (`_Resistance`, `_CircuitName`, `_ConnectionCheckTime`, `_EngineCheckTime`), **Blinking** (+ `_Period`, `_DutyCycle`, `_Blinking`), **Wiper** (+ `_PeriodicDelay`, `_PeriodicPhase`, `_NormalPhase`, `_FastPhase`), **Needle** (+ `_IsMechanical`, `_DefaultValue`, `_ValueSpeed`).

| # | Prefix in `[CarElectrics]` | Type | Activation Condition / What It Indicates | Output `InternalName` (`Real`) in XML |
| :-: | --- | :-: | --- | --- |
| 1 | `LowBeam` | Basic | `LowBeam` input (turns off when `HighBeam` is enabled) | `LowBeamIntensity` |
| 2 | `LowBeamIcon` | Basic | Mirrors `LowBeam` state | `LowBeamIconIntensity` |
| 3 | `HighBeam` | Basic | `HighBeam` input (with `LowBeam` on) or `HighBeamUnfixed` | `HighBeamIntensity` |
| 4 | `HighBeamIcon` | Basic | Mirrors `HighBeam` state | `HighBeamIconIntensity` |
| 5 | `ParkingLights` | Basic | `ParkingLights` input (position/marker lights) | `ParkingLightsIntensity` |
| 6 | `ParkingLightsIcon` | Basic | Mirrors `ParkingLights` state | `ParkingLightsIconIntensity` |
| 7 | `FrontFogLights` | Basic | `FrontFogLights` input (front fog lamps) | `FrontFogLightsIntensity` |
| 8 | `RearFogLights` | Basic | `RearFogLights` input (rear fog lamps) | `RearFogLightsIntensity` |
| 9 | `StopLight` | Basic | Brake pedal input `Brake > 0.01` | `StopLightIntensity` |
| 10 | `ReverseLight` | Basic | Reverse gear in `GearBox` (`< 0`) or `R` mode in `GearBoxLogic` | `ReverseLightIntensity` |
| 11 | `ParkingBrakeLight` | Basic | Parking brake engaged on `Brake*` (`> 0.01`) | `ParkingBrakeLightIntensity` |
| 12 | `RetarderLight` | Basic | `Retarder` input (`true`) | `RetarderLightIntensity` |
| 13 | `LeftTurnLight` | Blinking | `LeftTurnLight` input (overridden by `HazardLight`) | `LeftTurnLightIntensity` *(max with `HazardLight` and `EngineShutdownIcon`)* |
| 14 | `LeftTurnLightIcon` | Blinking | Mirrors `LeftTurnLight` | `LeftTurnLightIconIntensity` *(max with `HazardLightIcon` and `EngineShutdownIcon`)* |
| 15 | `RightTurnLight` | Blinking | `RightTurnLight` input (overridden by `HazardLight`) | `RightTurnLightIntensity` *(max with `HazardLight` and `EngineShutdownIcon`)* |
| 16 | `RightTurnLightIcon` | Blinking | Mirrors `RightTurnLight` | `RightTurnLightIconIntensity` *(max with `HazardLightIcon` and `EngineShutdownIcon`)* |
| 17 | `StartControlIcon` | Blinking | `StarterRelay` active (blinks if gearbox is not in neutral) | `StartControlIconIntensity` |
| 18 | `HeaterControlIcon` | Blinking | Key in `Ignition` with engine off (steady, then blinks) | `HeaterControlIconIntensity` |
| 19 | `HazardLight` | Blinking | Hazard flasher input `HazardLight` | *(merged into `LeftTurnLightIntensity` and `RightTurnLightIntensity`)* |
| 20 | `HazardLightIcon` | Blinking | Mirrors `HazardLight` state | `HazardLightIconIntensity` |
| 21 | `Dashboard` | Basic | `ParkingLights`, `LowBeam`, or latched `HighBeam` enabled | `DashboardIntensity` *(max with `AlwaysLitDashboard`)* |
| 22 | `AlwaysLitDashboard` | Basic | Always on when `HasAlwaysLitDashboard = true` | *(merged into `DashboardIntensity`)* |
| 23 | `SafetyBeltIcon` | Blinking | Seatbelt unfastened (`SafetyBelt = false`; blinks when buzzer sounds) | `SafetyBeltIconIntensity` |
| 24 | `SafetyBeltBeeper` | Basic | Driving unbuckled faster than `SafetyBeltWarningSpeed` or longer than `SafetyBeltWarningTime` | `SafetyBeltBeeperVolume` |
| 25 | `ParkingBrakeIcon` | Blinking | `ParkingBrake > 0.01` engaged (steady) or `BrokenBrake = true` (blinks) | `ParkingBrakeIconIntensity` |
| 26 | `BatteryIcon` | Basic | Alternator is not supplying current (`Generator` inactive) | `BatteryIconIntensity` |
| 27 | `Horn` | Basic | Horn input `Horn` | `HornVolume` |
| 28 | `FrontWindshieldWiper` | Wiper | `FrontWindshieldWiper` input (`0..3`) | `FrontWindshieldWiperPhase` (`[0..1]`), `FrontWindshieldWiperSpeed` ($1/\text{s}$) |
| 29 | `BackWindshieldWiper` | Wiper | `BackWindshieldWiper` input (`0..3`) | `BackWindshieldWiperPhase` (`[0..1]`), `BackWindshieldWiperSpeed` ($1/\text{s}$) |
| 30 | `FuelIcon` | Basic | Remaining fuel in `FuelTank` below `LowFuelLevel` | `FuelIconIntensity` |
| 31 | `FuelNeedle` | Needle | Fuel volume in `FuelTank` (L) | `Fuel` *(exported in schematics as `DashboardFuel`)* |
| 32 | `SpeedNeedle` | Needle | Speed from `Speedometer` (m/s) | `Speed` *(in schematics — `DashboardSpeed`)* |
| 33 | `RpmNeedle` | Needle | Crankshaft speed of `Engine` (RPM) | `RPM` *(in schematics — `DashboardRPM`)* |
| 34 | `CoolantTemperatureNeedle` | Needle | Coolant temperature of `Engine` (°C) | `CoolantTemperature` *(in schematics — `DashboardCoolantTemperature`)* |
| 35 | `OilNeedle` | Needle | Oil pressure of `Engine` (bar) | `OilPressure` *(in schematics — `DashboardOilPressure`)* |
| 36 | `GearBoxOilNeedle` | Needle | Gearbox oil pressure from `Pump` (bar) | `GearBoxOilPressure` |
| 37 | `VoltageNeedle` | Needle | Circuit voltage with minus sign (`-Voltage`, V) | `Voltage` *(in schematics — `DashboardVoltage`)* |
| 38 | `AmperageNeedle` | Needle | `Battery` current (A) | `Amperage` *(in schematics — `DashboardAmperage`)* |
| 39 | `PneumoPressure0Needle` | Needle | Vehicle output `PneumoPressure0` (bar) | `PneumoPressure0` |
| 40 | `PneumoPressure1Needle` | Needle | Vehicle output `PneumoPressure1` (bar) | `PneumoPressure1` |
| 41 | `OilIcon` | Basic | Engine oil pressure `< OilLowPressureThreshold` | `OilIconIntensity` |
| 42 | `GearBoxOilIcon` | Basic | Gearbox oil pressure `< GearBoxOilLowPressureThreshold` | `GearBoxOilIconIntensity` |
| 43 | `GearBoxOilHighPressureIcon` | Basic | Gearbox oil pressure `> GearBoxOilHighPressureThreshold` | `GearBoxOilHighPressureIconIntensity` |
| 44 | `OverheatIcon` | Basic | Engine coolant temperature `> OverheatTemperature` | `OverheatIconIntensity` |
| 45 | `Beeper` | Basic | Brake failure with engine running or engine overheating | `BeeperVolume` |
| 46 | `CheckIcons` | Basic | Startup self-check (`CheckIcons_ConnectionCheckTime`) | `CheckIconsIntensity` |
| 47 | `AbsDisableIcon` | Basic | ABS disabled in `AbsTcs` unit | `AbsDisableIconIntensity` |
| 48 | `AbsActiveIcon` | Blinking | ABS actively intervening in `AbsTcs` | `AbsActiveIconIntensity` |
| 49 | `TcsDisableIcon` | Basic | TCS disabled in `AbsTcs` unit | `TcsDisableIconIntensity` |
| 50 | `TcsActiveIcon` | Blinking | TCS actively intervening in `AbsTcs` | `TcsActiveIconIntensity` |
| 51 | `AirbagIcon` | Basic | Airbag self-check (`_ConnectionCheckTime`) | `AirbagIconIntensity` |
| 52 | `ExhaustSystemIcon` | Basic | Exhaust system self-check (`_EngineCheckTime`) | `ExhaustSystemIconIntensity` |
| 53 | `PowerSteeringIcon` | Basic | Engine RPM `< StableRPM / 2` or `BrokenPowerSteering` input | `PowerSteeringIconIntensity` |
| 54 | `EmergencyStopIcon` | Basic | Brake failure, power steering failure, or engine overheat | `EmergencyStopIconIntensity` |
| 55–58 | `LowAirPressureIcon0`..`3` | Basic | Pressure in pneumatic circuit `0..3` below `MinWorkPneumoPressure0..3` | **`LowPressureIcon0Intensity`** .. **`LowPressureIcon3Intensity`** |
| 59–62 | `DoorIcon0`..`3` | Basic | Corresponding pneumatic door `0..3` is open (`> 0.05`) | `DoorIcon0Intensity` .. `DoorIcon3Intensity` |
| 63 | `RetarderIcon` | Basic | `Retarder` input (`true`) | `RetarderIconIntensity` |
| 64 | `LowAirPressureIcon` | Basic | Low pressure in at least one of the 4 pneumatic circuits | **`LowPressureIconIntensity`** |
| 65 | `EngineShutdownIcon` | Blinking | `ShutdownEngine` input (`true`) | **`ShutdownEngineIconIntensity`** |
| 66 | `HighOilPressureIcon` | Basic | Engine oil pressure `> OilHighPressureThreshold` | `HighOilPressureIconIntensity` |
| 67–71 | `GearBoxIconN`/`D`/`R`/`1`/`2` | Basic | `GearBoxLogic` selector position (`N`, `D`, `R`, `D+1`, `D+2`) | `GearBoxIconNIntensity`, `GearBoxIconDIntensity`, `GearBoxIconRIntensity`, `GearBoxIcon1Intensity`, `GearBoxIcon2Intensity` |
| 72 | `LowTirePressureIcon` | Basic | Pressure in at least one tire `< CriticalPressure` | `LowTirePressureIconIntensity` |
| 73 | `BrakeOilLowLevelIcon` | Basic | Brake failure input `BrokenBrake = true` | `BrakeOilLowLevelIconIntensity` |
| 74–76 | `DiffLock0Icon`..`DiffLock2Icon` | Basic | Differential `0..2` is locked | **`DiffLockIcon0Intensity`** .. **`DiffLockIcon2Intensity`** |
| 77–79 | `DriveAxis0Icon`..`DriveAxis2Icon` | Basic | Driven axle coupling `0..2` engaged (`AxisRelay="true"`) | **`DriveAxisIcon0Intensity`** .. **`DriveAxisIcon2Intensity`** |
| 80 | `DownshiftAWDIcon` | Basic | All axles engaged and low-range transfer gear selected (`Reducer="true"`) | `DownshiftAWDIconIntensity` |
| 81 | `OverdriveAWDIcon` | Basic | All axles engaged and high-range transfer gear selected | `OverdriveAWDIconIntensity` |

*(Additionally, `CarElectrics` exposes `InternalName="EnvironmentTemperature"` of type `Real` — ambient air temperature in °C.)*

### 3.5. Important: Icons with Mismatched `.ini` Prefix and XML Output Names

For the following indicators, the parameter prefix in `Vehicle.ini` (`[CarElectrics]`) and the output `InternalName` in the XML `<OutputElements>` section are spelled differently — pay close attention:

| Parameter Prefix in `Vehicle.ini` (`[CarElectrics]`) | Exact Output `InternalName` in `<OutputElements>` (XML) | Difference |
| --- | --- | --- |
| `LowAirPressureIcon0_Resistance` .. `LowAirPressureIcon3_Resistance` | `LowPressureIcon0Intensity` .. `LowPressureIcon3Intensity` | The word `Air` is omitted in the XML output name |
| `LowAirPressureIcon_Resistance` | `LowPressureIconIntensity` | The word `Air` is omitted in the XML output name |
| `EngineShutdownIcon_Resistance` | `ShutdownEngineIconIntensity` | The words `Engine` and `Shutdown` are swapped |
| `DiffLock0Icon_Resistance` .. `DiffLock2Icon_Resistance` | `DiffLockIcon0Intensity` .. `DiffLockIcon2Intensity` | The index `0..2` precedes `Icon` in `.ini`, but follows `Icon` in XML |
| `DriveAxis0Icon_Resistance` .. `DriveAxis2Icon_Resistance` | `DriveAxisIcon0Intensity` .. `DriveAxisIcon2Intensity` | The index `0..2` precedes `Icon` in `.ini`, but follows `Icon` in XML |

### 3.6. Inputs (`<InputProcessors>`) and Boolean Outputs of `CarElectrics`

- **`Real` inputs (4 total):** `Brake` (`[0..1]` — activates `StopLight` when `> 0.01`), `ParkingBrake` (`[0..1]` — activates `ParkingBrakeIcon` when `> 0.01`), `FrontWindshieldWipersTimeDelay` and `BackWindshieldWipersTimeDelay` (`[0..∞)` s — intermittent wiper pause delay).
- **`Integer` inputs (2 total):** `FrontWindshieldWiper`, `BackWindshieldWiper` (`0` — off with return-to-park, `1` — intermittent, `2` — continuous, `3` — fast).
- **`Boolean` inputs (16 total):**
   - Lights and turn signals: `LowBeam`, `HighBeam` (active only together with `LowBeam` or `HighBeamUnfixed`), `HighBeamUnfixed` (flash-to-pass high beam), `ParkingLights`, `FrontFogLights`, `RearFogLights`, `LeftTurnLight`, `RightTurnLight`, `HazardLight`.
   - Systems and faults: `Horn` (horn), `SafetyBelt` (`true` — seatbelt fastened), `Retarder` (enables `RetarderLight` and `RetarderIcon`), `ShutdownEngine` (enables `EngineShutdownIcon`), `BrokenLighting` (disables all 12 exterior lights and dashboard backlight), `BrokenBrake` (brake system fault), `BrokenPowerSteering` (power steering failure).
- **Boolean outputs (`Type="Boolean"`, 4 total):** `TurnSignal` (`true` during the illuminated phase of turn signals/hazards — for flasher relay audio), `FrontWindshieldWiperForward` (`true` during the forward blade sweep `0 -> 1`), `HighBeam` (`true` when high beams are lit), `ParkingBrakeLight` (`true` when `ParkingBrakeIcon` is lit).

---

## 4. Ready-to-Use `[CarElectrics]` and `[Battery]` Example from CCD 2.0

```ini
[Battery]
VoltageMin = 10.5
VoltageMax = 14.7
Capacity = 90
Resistance = 0.0019

[CarElectrics]
DefaultCircuitName = IgnitionElectricCircuit
MinVoltage = 10.5
Voltage = 14.7
HasAlwaysLitDashboard = true
LowFuelLevel = 0.21
OilLowPressureThreshold = 0.98
SafetyBeltWarningSpeed = 20
HasBrokenBrakeBeeper = false
HasEngineOverheatBeeper = false

LowBeam_Resistance = 1.3
HighBeam_Resistance = 1.2
ParkingLights_Resistance = 7
ParkingLights_CircuitName = BatteryElectricCircuit
StopLight_Resistance = 7
StopLight_CircuitName = BatteryElectricCircuit
ReverseLight_Resistance = 5
FrontFogLights_Resistance = 7
RearFogLights_Resistance = 7
LeftTurnLight_Resistance = 7
LeftTurnLight_Period = 1.0
RightTurnLight_Resistance = 7
RightTurnLight_Period = 1.0
HazardLight_Resistance = 3.5
HazardLight_CircuitName = BatteryElectricCircuit
HazardLight_Period = 1.0

Dashboard_Resistance = 14
AlwaysLitDashboard_Resistance = 14
LowBeamIcon_Resistance = 40
HighBeamIcon_Resistance = 40
ParkingLightsIcon_Resistance = 40
LeftTurnLightIcon_Resistance = 40
RightTurnLightIcon_Resistance = 40
HazardLightIcon_Resistance = 40
HazardLightIcon_CircuitName = BatteryElectricCircuit
SafetyBeltIcon_Resistance = 40
SafetyBeltIcon_ConnectionCheckTime = 0.8
SafetyBeltBeeper_Resistance = 40
ParkingBrakeIcon_Resistance = 40
ParkingBrakeIcon_ConnectionCheckTime = 1.1
BatteryIcon_Resistance = 40
FuelIcon_Resistance = 40
OilIcon_Resistance = 40
OverheatIcon_Resistance = 40
CheckIcons_Resistance = 40
CheckIcons_ConnectionCheckTime = 1.8
ExhaustSystemIcon_Resistance = 40
ExhaustSystemIcon_EngineCheckTime = 0.5
PowerSteeringIcon_Resistance = 40
AirbagIcon_Resistance = 40
AirbagIcon_ConnectionCheckTime = 5.0
AirbagIcon_EngineCheckTime = 0.5
AbsActiveIcon_Resistance = 40
AbsActiveIcon_Period = 0.3
Horn_Resistance = 5
Horn_CircuitName = BatteryElectricCircuit
FrontWindshieldWiper_Resistance = 2.5
FrontWindshieldWiper_PeriodicDelay = 1.0
FrontWindshieldWiper_PeriodicPhase = 2.0
FrontWindshieldWiper_NormalPhase = 2.0
FrontWindshieldWiper_FastPhase = 1.0
BackWindshieldWiper_Resistance = 4
FuelNeedle_Resistance = 100
FuelNeedle_IsMechanical = true
SpeedNeedle_Resistance = 100
RpmNeedle_Resistance = 100
CoolantTemperatureNeedle_Resistance = 100
CoolantTemperatureNeedle_DefaultValue = 50
```
