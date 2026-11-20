# 04. Бортовая электросеть и приборная панель (`CarElectrics`) в City Car Driving 2.0

**Навигация:** [README](./README.md) | [00. Быстрый старт](./00_Quick_Start.md) | [01. Архитектура и конфиги](./01_Architecture_and_Config.md) | [02. Именованный ввод/вывод](./02_Named_IO_and_States.md) | [03. Трансмиссия](./03_Components_TorqueTrain.md) | **04. Электрика** | [05. Шасси и подсистемы](./05_Components_Chassis_and_Subsystems.md)

## Содержание

1. [Схема электросети автомобиля в City Car Driving 2.0](#1-схема-электросети-автомобиля-в-city-car-driving-20)
2. [Компоненты `ElectricCircuit`, `Battery` и `ElectricElement`](#2-компоненты-electriccircuit-battery-и-electricelement)
3. [Бортовая электрика, светотехника и приборная панель `CarElectrics`](#3-бортовая-электрика-светотехника-и-приборная-панель-carelectrics)
4. [Готовый пример секции `[CarElectrics]` и `[Battery]` из CCD 2.0](#4-готовый-пример-секции-carelectrics-и-battery-из-ccd-20)

---

## 1. Схема электросети автомобиля в City Car Driving 2.0

Во всех стандартных XML-схемах (`Common/Vehicles/MotorCars/Schemes/*.xml`) бортовая сеть постоянного тока строится из трёх узловых цепей (`Class="ElectricCircuit"`):

1. **`BatteryElectricCircuit`** — неотключаемая силовая цепь аккумулятора (`Battery`) и стартера (`Starter`). Питает приборы, работающие при вынутом ключе зажигания (аварийка `HazardLight`, габариты `ParkingLights`, стоп-сигналы `StopLight`, звуковой сигнал `Horn`).
2. **`GeneratorElectricCircuit`** — цепь генератора (`Generator`), постоянно соединённая с `BatteryElectricCircuit`.
3. **`IgnitionElectricCircuit`** — коммутируемая цепь зажигания (клемма 15). В XML она подключается к `BatteryElectricCircuit` с атрибутом **`Connected="false"`**:

```xml
<Link Holder="BatteryElectricCircuit" Reference="Battery" />
<Link Holder="BatteryElectricCircuit" Reference="Starter" />
<Link Holder="BatteryElectricCircuit" Reference="CarElectrics" />
<Link Holder="BatteryElectricCircuit" Reference="GeneratorElectricCircuit" />
<Link Holder="GeneratorElectricCircuit" Reference="Generator" />
<Link Holder="BatteryElectricCircuit" Reference="IgnitionElectricCircuit" Connected="false" />
<Link Holder="IgnitionElectricCircuit" Reference="Engine" /> <!-- Только для EngineGazoline! -->
<Link Holder="IgnitionElectricCircuit" Reference="CarElectrics" />
<Link Holder="IgnitionKey" Reference="IgnitionElectricCircuit" />
```

Когда водитель поворачивает ключ зажигания (`IgnitionKey`, `Class="IgnitionKeyProcessor"`) в положение `1` (`Ignition`) или `2` (`Starter`), `IgnitionKey` замыкает связь `BatteryElectricCircuit -> IgnitionElectricCircuit`, подавая напряжение бортовой сети на приборы зажигания, фары, поворотники, дворники, приборную панель и свечи зажигания бензинового ДВС.

> [!IMPORTANT]
> Чтобы приборы из `[CarElectrics]` получали питание от конкретной цепи (например, `IgnitionElectricCircuit` или `BatteryElectricCircuit`), в `<Links>` обязательно должна быть прописана связь `<Link Holder="ИмяЦепи" Reference="CarElectrics" />`, а в `Vehicle.ini` — указано имя этой цепи в `DefaultCircuitName` или `<Префикс>_CircuitName`.

---

## 2. Компоненты `ElectricCircuit`, `Battery` и `ElectricElement`

### 2.1. Электрическая цепь (`Class="ElectricCircuit"`)

Рассчитывает общее напряжение параллельно включённых источников ЭДС (`Battery`, `Generator`) и потребителей по закону Ома с учётом их внутреннего сопротивления. Параметров в `Vehicle.ini` не имеет.

- **Связи `<Links>` (`Holder="ИмяЦепи"`):** принимает `Battery`, `Generator`, `Starter`, `Engine` (`EngineGazoline`), `CarElectrics`, одиночные `ElectricElement` и дочерние `ElectricCircuit` (с опциональным атрибутом `Connected="true|false"`).
- **Вход (`InternalName`):** `Master` (`Boolean`) — замыкает (`true`) или размыкает (`false`) связь данной цепи с родительской цепью. **Выход (`InternalName`):** `Voltage` (`Real`, В) — текущее напряжение в цепи.

### 2.2. Аккумуляторная батарея (`[Battery]`, `Class="Battery"`)

Источник постоянного тока, который разряжается при нагрузке (особенно при пуске стартера) и заряжается, когда напряжение генератора в сети превышает текущую ЭДС аккумулятора.

| Ключ в `Vehicle.ini` (`[Battery]`) | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | :-: | :-: | --- |
| `VoltageMin` | `Real` | `10.0` | В | Минимальная ЭДС при полностью разряженном аккумуляторе (`10.0–10.5`). |
| `VoltageMax` | `Real` | `12.0` | В | Максимальная ЭДС полностью заряженной АКБ (`12.6–14.7`). |
| `Capacity` | `Real` | `60.0` | А·ч | Ёмкость аккумулятора в ампер-часах (`55–100`). |
| `Resistance` | `Real` | `0.02` | Ом | Базовое внутреннее сопротивление АКБ (`0.002–0.02`). |
| `DischargeResistance` | `Real` | `Resistance` | Ом | Внутреннее сопротивление в режиме отдачи тока (разряда). |
| `ChargeResistance` | `Real` | `Resistance` | Ом | Внутреннее сопротивление в режиме приёма тока (заряда от генератора). |

- **Входы (`InternalName`):** `Battery` (`Boolean`, по умолчанию `true` — выключатель массы АКБ), `EnableUnlimited` (`Boolean` — режим бесконечного заряда `UnlimitedBattery`).
- **Выходы (`InternalName`):** `Amperage` (`Real`, А: `> 0` — разряд, `< 0` — заряд), `VoltageMin` (`Real`, В), `CapacityRelative` (`Real`, `[0..1]`), `BatteryActive` (`Boolean` — `true`, если АКБ разряжается).

### 2.3. Одиночный электропотребитель (`Class="ElectricElement"`)

Универсальный резистивный прибор (обогреватель, дополнительный фонарь, спецсигнал), подключаемый к `ElectricCircuit`.

- **Параметры `.ini`:** `Resistance` (`5.0` Ом — сопротивление во включённом состоянии), `MinVoltage` (`9.0` В — напряжение начала работы), `NormalVoltage` (`12.0` В — напряжение 100% яркости/мощности).
- **Вход (`InternalName`):** `Enabled` (`Boolean`). **Выходы (`InternalName`):** `Intensity` (`Real`, `[0..∞)`), `Enabled` (`Boolean`).

---

## 3. Бортовая электрика, светотехника и приборная панель `CarElectrics`

Компонент `<Component Name="CarElectrics" Class="CarElectrics" />` (секция **`[CarElectrics]`** в `Vehicle.ini`) объединяет всю автомобильную светотехнику, поворотники, стеклоочистители, звуковой сигнал, зуммеры, контрольные лампы и стрелочные указатели приборной панели.

### 3.1. Связи `<Links>` с `CarElectrics` в XML

Чтобы `CarElectrics` автоматически считывал показания агрегатов для стрелок и сигнальных ламп, в секции `<Links>` задаются связи `<Link Holder="CarElectrics" Reference="ИмяКомпонента" />`:

| Целевой компонент (`Reference`) | XML-атрибут в `<Link>` | Какие приборы и лампы в `CarElectrics` оживляет эта связь |
| --- | :-: | --- |
| `Engine` (`EngineGazoline` / `EngineDiesel`) | — | Тахометр `RpmNeedle`, термометр ОЖ `CoolantTemperatureNeedle`, манометр `OilNeedle`, лампы `OilIcon`, `HighOilPressureIcon`, `OverheatIcon`, `PowerSteeringIcon`, `ParkingBrakeIcon`, `EmergencyStopIcon`, `BrakeOilLowLevelIcon`, зуммер `Beeper`. |
| `Speedometer` | — | Спидометр `SpeedNeedle`, лампа и зуммер непристёгнутого ремня `SafetyBeltIcon`, `SafetyBeltBeeper`. |
| `FuelTank` (`Class="Tank"`) | — | Указатель уровня топлива `FuelNeedle` (в литрах) и лампа резерва топлива `FuelIcon`. |
| `Generator` | — | Контрольная лампа отсутствия заряда АКБ `BatteryIcon` (гаснет при выходе генератора на рабочий режим). |
| `Battery` | — | Стрелка амперметра `AmperageNeedle`. |
| `IgnitionKey` (`IgnitionKeyProcessor`) | — | Лампа свечей накаливания / предпускового подогрева `HeaterControlIcon`. |
| `StarterRelay` (`Class="Relay"`) | — | Лампа контроля пуска стартера `StartControlIcon`. |
| `GearBox` (в `*Manual.xml`) | — | Фонарь заднего хода `ReverseLight` (включается при передаточном числе `< 0`). |
| `GearBox` | `StartIconGearBox="true"` | Контроль нейтрали КПП для режима мигания лампы `StartControlIcon`. |
| `GearBox` | `Reducer="true"` | Раздаточная коробка / демультипликатор для иконок `DownshiftAWDIcon` и `OverdriveAWDIcon`. |
| `Relay` (муфты мостов) | `AxisRelay="true"` | Иконки включения ведущих мостов `DriveAxis0..2Icon`, `DownshiftAWDIcon`, `OverdriveAWDIcon` *(требуют также связи с `Reducer="true"`)*. |
| `GearBoxLogic` (в `*Auto.xml`) | — | Фонарь заднего хода `ReverseLight` и индикаторы режимов АКПП `GearBoxIconN`, `GearBoxIconR`, `GearBoxIconD`, `GearBoxIcon1`, `GearBoxIcon2`. |
| `GearBoxAutomaticPump` (`Class="Pump"`) | — | Манометр и лампы давления масла КПП: `GearBoxOilNeedle`, `GearBoxOilIcon`, `GearBoxOilHighPressureIcon`. |
| `AbsTcs` | — | Контрольные лампы `AbsDisableIcon`, `AbsActiveIcon`, `TcsDisableIcon`, `TcsActiveIcon`. |
| `BrakeFrontLeft/Right`, `BrakeBackLeft/Right` | — | Сигнал стояночного тормоза `ParkingBrakeLight` (при затяжке ручника `> 0.01`). |
| `WheelFrontLeft/Right`, `WheelBackLeft/Right` | — | Лампа падения давления в шинах (TPMS) `LowTirePressureIcon`. |
| Приводы дверей (`ActuatorPneumatic2Chamber`) | — | Контрольные лампы открытых дверей автобуса/кузова `DoorIcon0` .. `DoorIcon3`. |

### 3.2. Общие параметры секции `[CarElectrics]` в `Vehicle.ini` (17 параметров)

| Ключ в `Vehicle.ini` (`[CarElectrics]`) | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | :-: | :-: | --- |
| `DefaultCircuitName` | `String` | `""` | имя цепи | Имя электроцепи по умолчанию для всех элементов (обычно `IgnitionElectricCircuit`). |
| `MinVoltage` | `Real` | `9.0` | В | Минимальное напряжение бортовой сети, ниже которого яркость ламп равна `0` (`0 < MinVoltage <= Voltage`). |
| `Voltage` | `Real` | `13.0` | В | Номинальное напряжение сети, при котором яркость ламп равна `1.0` (`13.0–14.7`). |
| `HasAlwaysLitDashboard` | `Boolean` | `false` | — | Если `true`, подсветка приборной панели `AlwaysLitDashboard` горит всегда при включённом зажигании (независимо от габаритов). |
| `LowFuelLevel` | `Real` | `0.1` | `[0..1]` | Доля остатка топлива в баке, ниже которой загорается лампа резерва `FuelIcon` (например, `0.15–0.21`). |
| `OilLowPressureThreshold` | `Real` | `0.6` | бар | Порог аварийно низкого давления масла ДВС для зажигания маслёнки `OilIcon`. |
| `OilHighPressureThreshold` | `Real` | `6.1` | бар | Порог чрезмерно высокого давления масла ДВС для `HighOilPressureIcon`. |
| `OilPressureConsiderEngine` | `Boolean` | `false` | — | Если `true`, лампы давления масла ДВС горят только при заведённом моторе; если `false` — горят и при включённом зажигании до пуска ДВС. |
| `GearBoxOilLowPressureThreshold` | `Real` | `1.0` | бар | Нижний аварийный порог давления масла КПП для `GearBoxOilIcon`. |
| `GearBoxOilHighPressureThreshold` | `Real` | `10.0` | бар | Верхний аварийный порог давления масла КПП для `GearBoxOilHighPressureIcon`. |
| `GearBoxOilPressureConsiderEngine` | `Boolean` | `false` | — | Учитывать ли факт работы ДВС для ламп давления масла КПП. |
| `HasBrokenBrakeBeeper` | `Boolean` | `true` | — | Включать ли аварийный зуммер `Beeper` при отказе тормозов (`BrokenBrake = true`) и работающем ДВС. |
| `HasEngineOverheatBeeper` | `Boolean` | `true` | — | Включать ли аварийный зуммер `Beeper` при перегреве двигателя (`CoolantTemperature > OverheatTemperature`). |
| `HeaterControlIcon_BlinkingTime` | `Real` | `2.0` | с | Время непрерывного свечения иконки накала свечей `HeaterControlIcon` до перехода в мигающий режим. |
| `SafetyBeltMinSpeed` | `Real` | `2.0` | км/ч | Минимальная скорость движения, с которой начинает идти таймер езды без ремня безопасности. |
| `SafetyBeltWarningSpeed` | `Real` | `20.0` | км/ч | Скорость, при превышении которой без ремня немедленно включается зуммер `SafetyBeltBeeper` и мигание `SafetyBeltIcon`. |
| `SafetyBeltWarningTime` | `Real` | `60.0` | с | Время езды со скоростью выше `SafetyBeltMinSpeed` без ремня до включения зуммера `SafetyBeltBeeper`. |

### 3.3. Настройка ламп, фар, поворотников, дворников и стрелок через префиксы `<Префикс>_*`

Внутри секции `[CarElectrics]` каждый из 81 прибора настраивается параметрами вида `<ПрефиксЭлемента>_<Суффикс>`:

| Категория элементов | Суффикс параметра в `[CarElectrics]` | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | --- | :-: | :-: | --- |
| **Все 81 элемент** (фары, лампы, иконки, зуммеры, дворники, стрелки) | `<Префикс>_Resistance` | `Real` | **`0.0`** | Ом | Электрическое сопротивление включённого прибора (`1.2–7.0` для фар, `40` для ламп приборки, `100` для стрелок). |
|  | `<Префикс>_CircuitName` | `String` | `DefaultCircuitName` | имя цепи | Индивидуальная привязка прибора к электроцепи (например, `BatteryElectricCircuit` для `HazardLight`, `ParkingLights`, `StopLight`, `Horn`). |
|  | `<Префикс>_ConnectionCheckTime` | `Real` | `0.0` *(или $\infty$)* | с | Длительность тестового свечения лампы после подачи питания в цепь (самодиагностика приборной панели при повороте ключа). |
|  | `<Префикс>_EngineCheckTime` | `Real` | `0.0` | с | Длительность тестового свечения лампы **после запуска ДВС** (до погасания). |
| **Мигающие элементы** (`*TurnLight*`, `HazardLight*`, `SafetyBeltIcon`, `ParkingBrakeIcon`, `AbsActiveIcon`, `TcsActiveIcon`, `StartControlIcon`, `HeaterControlIcon`, `EngineShutdownIcon`) | `<Префикс>_Period` | `Real` | `0.6` (`0.0` для ABS/TCS) | с | Полный период цикла мигания (`1.0` — 60 вспышек/мин; при `0.0` горит непрерывно без мигания). |
|  | `<Префикс>_DutyCycle` | `Real` | `0.5` | `[0..1]` | Доля периода, в течение которой лампа включена (`0.5` — полпериода горит, полпериода погашена). |
|  | `<Префикс>_Blinking` | `Boolean` | `true` | — | Разрешено ли мигание элемента по умолчанию. |
| **Стеклоочистители (дворники)** (`FrontWindshieldWiper`, `BackWindshieldWiper`) | `<Префикс>_PeriodicDelay` | `Real` | `2.0` | с | Пауза между взмахами щёток в прерывистом режиме (`1`). |
|  | `<Префикс>_PeriodicPhase` | `Real` | `2.0` | с | Время полного взмаха туда-обратно (`0 -> 1 -> 0`) в прерывистом режиме (`1`). |
|  | `<Префикс>_NormalPhase` | `Real` | `2.0` | с | Время полного взмаха туда-обратно в постоянном режиме (`2`) и при возврате на парковку (`0`). |
|  | `<Префикс>_FastPhase` | `Real` | `1.0` | с | Время полного взмаха туда-обратно в быстром режиме (`3`). |
| **Стрелочные приборы** (`*Needle`) | `<Префикс>_IsMechanical` | `Boolean` | `false` | — | Если `true`, прибор механический — работает всегда без электропитания и не требует задания `_Resistance`. |
|  | `<Префикс>_DefaultValue` | `Real` | `0.0` | ед. шкалы | Показание стрелки при выключенном зажигании (например, `50` для `CoolantTemperatureNeedle`). |
|  | `<Префикс>_ValueSpeed` | `Real` | `5.0` | $1/\text{с}$ | Скорость плавного сглаживания движения стрелки к целевому значению. |

> [!IMPORTANT]
> **Два главных правила настройки элементов в `[CarElectrics]`:**
>
> 1. **Нулевое сопротивление = прибор отключён!** По умолчанию у всех 81 элементов `<Префикс>_Resistance = 0.0`, из-за чего они считаются неподключёнными к бортовой сети, и их выход в игру всегда равен **`0.0`**. Чтобы любая фара, контрольная лампа, зуммер, дворник или электрическая стрелка работали, **обязательно задайте для них в `[CarElectrics]` положительное сопротивление `<Префикс>_Resistance > 0`** (исключение — механические стрелки с `<Префикс>_IsMechanical = true`).
> 2. **Особенность `<Префикс>_EngineCheckTime`:** если задать для лампы `_EngineCheckTime > 0` (например, `ExhaustSystemIcon_EngineCheckTime = 0.5`), но не указать `_ConnectionCheckTime`, то `_ConnectionCheckTime` автоматически становится **бесконечным** — лампа будет гореть всё время при включённом зажигании и погаснет через `0.5 с` после запуска двигателя!

### 3.4. Сводная таблица всех 81 элемента `[CarElectrics]` и их выходов в XML

В колонке «Тип» указан набор поддерживаемых суффиксов: **Базовый** (`_Resistance`, `_CircuitName`, `_ConnectionCheckTime`, `_EngineCheckTime`), **Мигающий** (+ `_Period`, `_DutyCycle`, `_Blinking`), **Дворник** (+ `_PeriodicDelay`, `_PeriodicPhase`, `_NormalPhase`, `_FastPhase`), **Стрелка** (+ `_IsMechanical`, `_DefaultValue`, `_ValueSpeed`).

| № | Префикс в `[CarElectrics]` | Тип | Условие включения / что показывает | Имя выхода `InternalName` (`Real`) в XML |
| :-: | --- | :-: | --- | --- |
| 1 | `LowBeam` | Базовый | Вход `LowBeam` (гаснет при включении `HighBeam`) | `LowBeamIntensity` |
| 2 | `LowBeamIcon` | Базовый | Повторяет состояние `LowBeam` | `LowBeamIconIntensity` |
| 3 | `HighBeam` | Базовый | Вход `HighBeam` (при включённом `LowBeam`) или `HighBeamUnfixed` | `HighBeamIntensity` |
| 4 | `HighBeamIcon` | Базовый | Повторяет состояние `HighBeam` | `HighBeamIconIntensity` |
| 5 | `ParkingLights` | Базовый | Вход `ParkingLights` (габаритные огни) | `ParkingLightsIntensity` |
| 6 | `ParkingLightsIcon` | Базовый | Повторяет состояние `ParkingLights` | `ParkingLightsIconIntensity` |
| 7 | `FrontFogLights` | Базовый | Вход `FrontFogLights` (передние ПТФ) | `FrontFogLightsIntensity` |
| 8 | `RearFogLights` | Базовый | Вход `RearFogLights` (задние ПТФ) | `RearFogLightsIntensity` |
| 9 | `StopLight` | Базовый | Вход педали тормоза `Brake > 0.01` | `StopLightIntensity` |
| 10 | `ReverseLight` | Базовый | Задняя передача в `GearBox` (`< 0`) или режим `R` в `GearBoxLogic` | `ReverseLightIntensity` |
| 11 | `ParkingBrakeLight` | Базовый | Затянут стояночный тормоз на `Brake*` (`> 0.01`) | `ParkingBrakeLightIntensity` |
| 12 | `RetarderLight` | Базовый | Вход `Retarder` (`true`) | `RetarderLightIntensity` |
| 13 | `LeftTurnLight` | Мигающий | Вход `LeftTurnLight` (перекрывается `HazardLight`) | `LeftTurnLightIntensity` *(макс. с `HazardLight` и `EngineShutdownIcon`)* |
| 14 | `LeftTurnLightIcon` | Мигающий | Повторяет `LeftTurnLight` | `LeftTurnLightIconIntensity` *(макс. с `HazardLightIcon` и `EngineShutdownIcon`)* |
| 15 | `RightTurnLight` | Мигающий | Вход `RightTurnLight` (перекрывается `HazardLight`) | `RightTurnLightIntensity` *(макс. с `HazardLight` и `EngineShutdownIcon`)* |
| 16 | `RightTurnLightIcon` | Мигающий | Повторяет `RightTurnLight` | `RightTurnLightIconIntensity` *(макс. с `HazardLightIcon` и `EngineShutdownIcon`)* |
| 17 | `StartControlIcon` | Мигающий | Включено `StarterRelay` (мигает, если КПП не в нейтрали) | `StartControlIconIntensity` |
| 18 | `HeaterControlIcon` | Мигающий | Ключ в `Ignition` при незаведённом ДВС (горит, затем мигает) | `HeaterControlIconIntensity` |
| 19 | `HazardLight` | Мигающий | Вход аварийной сигнализации `HazardLight` | *(входит в `LeftTurnLightIntensity` и `RightTurnLightIntensity`)* |
| 20 | `HazardLightIcon` | Мигающий | Повторяет состояние `HazardLight` | `HazardLightIconIntensity` |
| 21 | `Dashboard` | Базовый | Включены `ParkingLights`, `LowBeam` или фиксированный `HighBeam` | `DashboardIntensity` *(макс. с `AlwaysLitDashboard`)* |
| 22 | `AlwaysLitDashboard` | Базовый | Постоянно при `HasAlwaysLitDashboard = true` | *(входит в `DashboardIntensity`)* |
| 23 | `SafetyBeltIcon` | Мигающий | Не пристёгнут ремень (`SafetyBelt = false`; мигает при зуммере) | `SafetyBeltIconIntensity` |
| 24 | `SafetyBeltBeeper` | Базовый | Езда без ремня быстрее `SafetyBeltWarningSpeed` или дольше `SafetyBeltWarningTime` | `SafetyBeltBeeperVolume` |
| 25 | `ParkingBrakeIcon` | Мигающий | Включён `ParkingBrake > 0.01` (горит) или `BrokenBrake = true` (мигает) | `ParkingBrakeIconIntensity` |
| 26 | `BatteryIcon` | Базовый | Генератор не отдаёт ток в сеть (`Generator` отключён) | `BatteryIconIntensity` |
| 27 | `Horn` | Базовый | Вход звукового сигнала `Horn` | `HornVolume` |
| 28 | `FrontWindshieldWiper` | Дворник | Вход `FrontWindshieldWiper` (`0..3`) | `FrontWindshieldWiperPhase` (`[0..1]`), `FrontWindshieldWiperSpeed` ($1/\text{с}$) |
| 29 | `BackWindshieldWiper` | Дворник | Вход `BackWindshieldWiper` (`0..3`) | `BackWindshieldWiperPhase` (`[0..1]`), `BackWindshieldWiperSpeed` ($1/\text{с}$) |
| 30 | `FuelIcon` | Базовый | Остаток топлива в `FuelTank` ниже `LowFuelLevel` | `FuelIconIntensity` |
| 31 | `FuelNeedle` | Стрелка | Объём топлива в `FuelTank` (л) | `Fuel` *(в схемах выводится как `DashboardFuel`)* |
| 32 | `SpeedNeedle` | Стрелка | Скорость от `Speedometer` (м/с) | `Speed` *(в схемах — `DashboardSpeed`)* |
| 33 | `RpmNeedle` | Стрелка | Обороты коленвала `Engine` (об/мин) | `RPM` *(в схемах — `DashboardRPM`)* |
| 34 | `CoolantTemperatureNeedle` | Стрелка | Температура ОЖ `Engine` (°C) | `CoolantTemperature` *(в схемах — `DashboardCoolantTemperature`)* |
| 35 | `OilNeedle` | Стрелка | Давление масла `Engine` (бар) | `OilPressure` *(в схемах — `DashboardOilPressure`)* |
| 36 | `GearBoxOilNeedle` | Стрелка | Давление масла КПП от `Pump` (бар) | `GearBoxOilPressure` |
| 37 | `VoltageNeedle` | Стрелка | Напряжение цепи со знаком минус (`-Voltage`, В) | `Voltage` *(в схемах — `DashboardVoltage`)* |
| 38 | `AmperageNeedle` | Стрелка | Ток аккумулятора `Battery` (А) | `Amperage` *(в схемах — `DashboardAmperage`)* |
| 39 | `PneumoPressure0Needle` | Стрелка | Выход автомобиля `PneumoPressure0` (бар) | `PneumoPressure0` |
| 40 | `PneumoPressure1Needle` | Стрелка | Выход автомобиля `PneumoPressure1` (бар) | `PneumoPressure1` |
| 41 | `OilIcon` | Базовый | Давление масла ДВС `< OilLowPressureThreshold` | `OilIconIntensity` |
| 42 | `GearBoxOilIcon` | Базовый | Давление масла КПП `< GearBoxOilLowPressureThreshold` | `GearBoxOilIconIntensity` |
| 43 | `GearBoxOilHighPressureIcon` | Базовый | Давление масла КПП `> GearBoxOilHighPressureThreshold` | `GearBoxOilHighPressureIconIntensity` |
| 44 | `OverheatIcon` | Базовый | Температура ОЖ ДВС `> OverheatTemperature` | `OverheatIconIntensity` |
| 45 | `Beeper` | Базовый | Отказ тормозов при работающем ДВС или перегрев ДВС | `BeeperVolume` |
| 46 | `CheckIcons` | Базовый | Самодиагностика при пуске (`CheckIcons_ConnectionCheckTime`) | `CheckIconsIntensity` |
| 47 | `AbsDisableIcon` | Базовый | Отключена ABS в блоке `AbsTcs` | `AbsDisableIconIntensity` |
| 48 | `AbsActiveIcon` | Мигающий | Активно срабатывает ABS в `AbsTcs` | `AbsActiveIconIntensity` |
| 49 | `TcsDisableIcon` | Базовый | Отключена TCS в блоке `AbsTcs` | `TcsDisableIconIntensity` |
| 50 | `TcsActiveIcon` | Мигающий | Активно срабатывает TCS в `AbsTcs` | `TcsActiveIconIntensity` |
| 51 | `AirbagIcon` | Базовый | Самодиагностика подушек безопасности (`_ConnectionCheckTime`) | `AirbagIconIntensity` |
| 52 | `ExhaustSystemIcon` | Базовый | Самодиагностика системы выпуска (`_EngineCheckTime`) | `ExhaustSystemIconIntensity` |
| 53 | `PowerSteeringIcon` | Базовый | Обороты ДВС `< StableRPM / 2` или вход `BrokenPowerSteering` | `PowerSteeringIconIntensity` |
| 54 | `EmergencyStopIcon` | Базовый | Отказ тормозов, отказ ЭУР/ГУР или перегрев ДВС | `EmergencyStopIconIntensity` |
| 55–58 | `LowAirPressureIcon0`..`3` | Базовый | Давление в пневмоконтуре `0..3` ниже `MinWorkPneumoPressure0..3` | **`LowPressureIcon0Intensity`** .. **`LowPressureIcon3Intensity`** |
| 59–62 | `DoorIcon0`..`3` | Базовый | Открыта соответствующая пневмодверь `0..3` (`> 0.05`) | `DoorIcon0Intensity` .. `DoorIcon3Intensity` |
| 63 | `RetarderIcon` | Базовый | Вход `Retarder` (`true`) | `RetarderIconIntensity` |
| 64 | `LowAirPressureIcon` | Базовый | Низкое давление хотя бы в одном из 4 пневмоконтуров | **`LowPressureIconIntensity`** |
| 65 | `EngineShutdownIcon` | Мигающий | Вход `ShutdownEngine` (`true`) | **`ShutdownEngineIconIntensity`** |
| 66 | `HighOilPressureIcon` | Базовый | Давление масла ДВС `> OilHighPressureThreshold` | `HighOilPressureIconIntensity` |
| 67–71 | `GearBoxIconN`/`D`/`R`/`1`/`2` | Базовый | Положение селектора `GearBoxLogic` (`N`, `D`, `R`, `D+1`, `D+2`) | `GearBoxIconNIntensity`, `GearBoxIconDIntensity`, `GearBoxIconRIntensity`, `GearBoxIcon1Intensity`, `GearBoxIcon2Intensity` |
| 72 | `LowTirePressureIcon` | Базовый | Давление хотя бы в одном колесе `< CriticalPressure` | `LowTirePressureIconIntensity` |
| 73 | `BrakeOilLowLevelIcon` | Базовый | Вход отказа тормозов `BrokenBrake = true` | `BrakeOilLowLevelIconIntensity` |
| 74–76 | `DiffLock0Icon`..`DiffLock2Icon` | Базовый | Заблокирован дифференциал `0..2` | **`DiffLockIcon0Intensity`** .. **`DiffLockIcon2Intensity`** |
| 77–79 | `DriveAxis0Icon`..`DriveAxis2Icon` | Базовый | Включена муфта ведущего моста `0..2` (`AxisRelay="true"`) | **`DriveAxisIcon0Intensity`** .. **`DriveAxisIcon2Intensity`** |
| 80 | `DownshiftAWDIcon` | Базовый | Включены все мосты и пониженная передача раздатки (`Reducer="true"`) | `DownshiftAWDIconIntensity` |
| 81 | `OverdriveAWDIcon` | Базовый | Включены все мосты и повышенная передача раздатки | `OverdriveAWDIconIntensity` |

*(Дополнительно `CarElectrics` предоставляет выход `InternalName="EnvironmentTemperature"` типа `Real` — температура окружающего воздуха в °C).*

### 3.5. Важно: иконки с несовпадающими именами префикса `.ini` и выхода XML

У следующих индикаторов имя префикса в `Vehicle.ini` (`[CarElectrics]`) и имя выхода `InternalName` в `<OutputElements>` XML-схемы пишутся по-разному — будьте внимательны:

| Префикс параметра в `Vehicle.ini` (`[CarElectrics]`) | Точное имя выхода `InternalName` в `<OutputElements>` (XML) | В чём отличие |
| --- | --- | --- |
| `LowAirPressureIcon0_Resistance` .. `LowAirPressureIcon3_Resistance` | `LowPressureIcon0Intensity` .. `LowPressureIcon3Intensity` | В имени выхода XML пропущено слово `Air` |
| `LowAirPressureIcon_Resistance` | `LowPressureIconIntensity` | В имени выхода XML пропущено слово `Air` |
| `EngineShutdownIcon_Resistance` | `ShutdownEngineIconIntensity` | Слова `Engine` и `Shutdown` переставлены местами |
| `DiffLock0Icon_Resistance` .. `DiffLock2Icon_Resistance` | `DiffLockIcon0Intensity` .. `DiffLockIcon2Intensity` | Индекс `0..2` в `.ini` стоит перед `Icon`, а в XML — после `Icon` |
| `DriveAxis0Icon_Resistance` .. `DriveAxis2Icon_Resistance` | `DriveAxisIcon0Intensity` .. `DriveAxisIcon2Intensity` | Индекс `0..2` в `.ini` стоит перед `Icon`, а в XML — после `Icon` |

### 3.6. Входы (`<InputProcessors>`) и булевы выходы `CarElectrics`

- **Входы типа `Real` (4 шт.):** `Brake` (`[0..1]` — стоп-сигналы `StopLight` при `> 0.01`), `ParkingBrake` (`[0..1]` — лампа ручника `ParkingBrakeIcon` при `> 0.01`), `FrontWindshieldWipersTimeDelay` и `BackWindshieldWipersTimeDelay` (`[0..∞)` с — пауза прерывистого режима дворников).
- **Входы типа `Integer` (2 шт.):** `FrontWindshieldWiper`, `BackWindshieldWiper` (`0` — выкл. с доводкой в парковку, `1` — прерывистый, `2` — постоянный, `3` — быстрый).
- **Входы типа `Boolean` (16 шт.):**
   - Свет и поворотники: `LowBeam`, `HighBeam` (включается только вместе с `LowBeam` или `HighBeamUnfixed`), `HighBeamUnfixed` (моргание дальним), `ParkingLights`, `FrontFogLights`, `RearFogLights`, `LeftTurnLight`, `RightTurnLight`, `HazardLight`.
   - Системы и отказы: `Horn` (клаксон), `SafetyBelt` (`true` — ремень пристёгнут), `Retarder` (включает `RetarderLight` и `RetarderIcon`), `ShutdownEngine` (включает `EngineShutdownIcon`), `BrokenLighting` (отказ всех 12 фар, фонарей и подсветки приборки), `BrokenBrake` (авария тормозов), `BrokenPowerSteering` (отказ усилителя руля).
- **Булевы выходы (`Type="Boolean"`, 4 шт.):** `TurnSignal` (`true` в фазе свечения поворотников/аварийки — для звука реле), `FrontWindshieldWiperForward` (`true` при прямом ходе щёток `0 -> 1`), `HighBeam` (`true` при горящем дальнем свете), `ParkingBrakeLight` (`true` при горящей лампе `ParkingBrakeIcon`).

---

## 4. Готовый пример секции `[CarElectrics]` и `[Battery]` из CCD 2.0

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
