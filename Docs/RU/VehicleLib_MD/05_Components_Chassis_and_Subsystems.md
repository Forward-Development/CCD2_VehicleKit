# 05. Компоненты шасси и подсистем (`Suspension`, `Input`, `Output`, `Pneumatics`, `Hydraulics`, `Aerodynamics`, `ExternalForce`)

**Навигация:** [README](./README.md) | [00. Быстрый старт](./00_Quick_Start.md) | [01. Архитектура и конфиги](./01_Architecture_and_Config.md) | [02. Именованный ввод/вывод](./02_Named_IO_and_States.md) | [03. Трансмиссия](./03_Components_TorqueTrain.md) | [04. Электрика](./04_Components_Electrics.md) | **05. Шасси и подсистемы**

---

## Содержание

- [1. Подвеска и рулевое управление](#1-подвеска-и-рулевое-управление)
   - [1.1. `Suspension1Spring` — пружинно-амортизаторная подвеска](#11-suspension1spring--пружинно-амортизаторная-подвеска)
   - [1.2. `Suspension1Constant` — жёсткая подвеска постоянной нагрузки](#12-suspension1constant--жёсткая-подвеска-постоянной-нагрузки)
   - [1.3. `Steering2Ackermann` — рулевая трапеция Аккермана](#13-steering2ackermann--рулевая-трапеция-аккермана)
   - [1.4. `SteeringSimple` — симметричное рулевое управление](#14-steeringsimple--симметричное-рулевое-управление)
- [2. Контроллеры управления (`Input`)](#2-контроллеры-управления-input)
   - [2.1. `FuelControllerComputer` (`[Idler]`) и `FuelControllerMechanical`](#21-fuelcontrollercomputer-секция-idler-и-fuelcontrollermechanical)
   - [2.2. `AbsTcs` — блок систем ABS, TCS и регулятора тормозных сил EBD](#22-abstcs--блок-систем-abs-tcs-и-регулятора-тормозных-сил-ebd)
   - [2.3. `ClutchInputProcessor` — нелинейный контроллер педали сцепления](#23-clutchinputprocessor--нелинейный-контроллер-педали-сцепления)
   - [2.4. `GearBoxAutomaticLogic` (`[GearBoxLogic]`) и `GearBoxAutomaticLogicBus`](#24-gearboxautomaticlogic-секция-gearboxlogic-и-gearboxautomaticlogicbus)
   - [2.5. `IgnitionKeyProcessor` — контроллер замка зажигания](#25-ignitionkeyprocessor--контроллер-замка-зажигания)
- [3. Телеметрия и обратная связь на руле (`Output`)](#3-телеметрия-и-обратная-связь-на-руле-output)
   - [3.1. `PowerSteering` — усилитель руля (ГУР/ЭУР) и Force Feedback](#31-powersteering--усилитель-руля-гурэур-и-force-feedback)
   - [3.2. `Speedometer` — спидометр и одометр](#32-speedometer--спидометр-и-одометр)
   - [3.3. `FuelConsumptionMeter` — датчик среднего расхода топлива](#33-fuelconsumptionmeter--датчик-среднего-расхода-топлива)
- [4. Наддув, пневматика, баки, аэродинамика и внешние силы](#4-наддув-пневматика-баки-аэродинамика-и-внешние-силы)
   - [4.1. `Turbocharger` — турбокомпрессор наддува ДВС](#41-turbocharger--турбокомпрессор-наддува-двс)
   - [4.2. Пневмосистема: `Receiver`, `ValveMultiCircuit`, `ActuatorPneumatic1/2Chamber`](#42-пневмосистема-receiver-valvemulticircuit-actuatorpneumatic1chamber-actuatorpneumatic2chamber)
   - [4.3. `Tank` — топливный и масляный бак](#43-tank--топливный-и-масляный-бак)
   - [4.4. `AerodynamicsCarSimple` — аэродинамика кузова](#44-aerodynamicscarsimple--аэродинамика-кузова)
   - [4.5. `ExternalThruster` — внешний линейный толкатель](#45-externalthruster--внешний-линейный-толкатель)

---

## Общие правила конфигурации в `Vehicle.ini` и XML-схемах

1. **Строгий синтаксис массивов и векторов в `Vehicle.ini`:** все списки чисел и 3D-векторы записываются **только через точку с запятой `;` без пробела перед `;`** (`Mount = 1.63; 0.85; 0.4455`). Запятые `,` не поддерживаются, а комбинация `пробел + ;` (`" ;"`) воспринимается парсером `.ini` как начало комментария и обрезает хвост строки!
2. **Связи (`<Links>`) и сигналы в XML:** связи между компонентами задаются тегом `<Link Holder="ИмяХранителя" Reference="ИмяЦели" />`. Внешние входы, выходы и состояния связываются с внутренними именами каналов (`InternalName`) в секциях `<InputProcessors>`, `<OutputElements>` и `<StateElements>`.

---

## 1. Подвеска и рулевое управление

### 1.1. `Suspension1Spring` — пружинно-амортизаторная подвеска

В стандартных схемах CCD 2.0 (`Common/Vehicles/MotorCars/Schemes/*.xml`) используется для всех четырёх стоек автомобиля: **`[SuspensionFrontLeft]`**, **`[SuspensionFrontRight]`**, **`[SuspensionBackLeft]`**, **`[SuspensionBackRight]`** (задняя ось всегда именуется **`Back`**, а не `Rear`!).

- **Связи в XML (`<Links>`):** каждая стойка связывается со своим колесом (`<Link Holder="SuspensionFrontLeft" Reference="WheelFrontLeft" />`). Входящая связь от рулевого механизма (`Steering`) подключается только к передним управляемым стойкам.
- **Привязка к кузову:** параметр `BodyName = Body` указывает имя главного физического меша кузова в Blueprint автомобиля.
- **Система координат в `Vehicle.ini`:** правая локальная система координат кузова в **метрах**:
   - **`X` — Вперёд** (`+X` — передняя ось, `-X` — задняя ось);
   - **`Y` — Влево** (`+Y` — левый борт, `-Y` — правый борт, противоположно оси `Y` в Unreal Engine 5);
   - **`Z` — Вверх**. Направление рабочего хода стойки фиксировано вертикально вниз вдоль вектора `Direction = 0; 0; -1`.

#### Точная формула перевода координат `Mount` из Blueprint UE5

В Blueprint автомобиля компоненты `SuspensionFrontLeft`, `SuspensionFrontRight`, `SuspensionBackLeft`, `SuspensionBackRight` размещаются в проектных центрах колёс при нормальной статической посадке с локальными координатами $(X_{\text{UE}},\; Y_{\text{UE}},\; Z_{\text{UE}})$ в **сантиметрах**.

> [!IMPORTANT]
> **Физический смысл точки `Mount` и формула `Mount.Z`:**
>  В компоненте `Suspension1Spring` вектор `Mount` задаёт положение центра колеса при **полном сжатии** пружины (`SpringSag = 1.0`), а текущее положение центра колеса отсчитывается от `Mount` вниз вдоль `Direction = 0; 0; -1`:
>
> $$\vec{P}_{\text{wheel}} = \text{Mount} + (1 - \text{SpringSag}) \cdot \text{Length} \cdot (0;\; 0;\; -1)$$
>
> При нормальной статической просадке `NormalSag` (обычно `0.33`, т.е. пружина сжата на $33\%$ хода) колесо опущено от точки `Mount` вниз на **оставшийся ход отбоя** $(1 - \text{NormalSag}) \cdot \text{Length} = 0.67 \cdot \text{Length}$!
>
> $$\text{Mount} = \left(\frac{X_{\text{UE}}}{100};\quad -\frac{Y_{\text{UE}}}{100};\quad \frac{Z_{\text{UE}}}{100} + (1 - \text{NormalSag}) \cdot \text{Length}\right)$$
>
> *Пример:* для `SuspensionFrontLeft` в точке `(163.0, -85.0, 31.15)` см при `Length = 0.20` м и `NormalSag = 0.33` получаем $\text{Mount}_Z = 0.3115 + 0.67 \cdot 0.20 = 0.4455\text{ м}$ и запись `Mount = 1.63; 0.85; 0.4455`. *(Ошибочная подстановка `+ NormalSag * Length` задирает кузов вверх на лишние $(1 - 2\cdot\text{NormalSag})\cdot\text{Length} \approx 6.5\text{ см}$!).*

#### Учёт кривой прогрессии `SpringProgression.csv` и угловой жёсткости крена

1. **Формула силы пружины и амортизатора:** при сжатии $x = \text{SpringSag} \cdot \text{Length}$ (м) и скорости удлинения $v$ (м/с, где $v > 0$ — отбой, $v \le 0$ — сжатие):
   $$F_{\text{spring}} = x \cdot \text{SpringCoeff} \cdot k_{\text{prog}}(\text{SpringSag}) + \text{PreloadForce} \cdot \text{Clamp}\left(\frac{x}{\text{PreloadRampBuffer}},\, 0,\, 1\right), \qquad F_{\text{damp}} = -\frac{v \cdot \text{DampCoeff}}{v > 0 \,?\; 1 : 3}$$
   На ходе сжатия `DampCoeff` автоматически делится на `3` (отбой в 3 раза жёстче сжатия).
2. **Множитель `Common/Vehicles/MotorCars/Tables/SpringProgression.csv` и подбор `SpringCoeff` / `PreloadForce` / `DampCoeff`:** стандартная кривая прогрессии (`0 -> 0.55 -> 0.7 -> 0.85 -> 0.95 -> 1.0`) при сплайн-интерполяции в точке `NormalSag = 0.33` даёт множитель **`0.654`** относительно линейного расчёта. При статической нагрузке на одно колесо $F_{\text{stat}} = m_{\text{corner}} \cdot 9.81$ параметры пружины и амортизатора связываются соотношениями:
   $$F_{\text{stat}} = \text{PreloadForce} + \text{SpringCoeff} \cdot \text{Length} \cdot \text{NormalSag} \cdot 0.654, \qquad \text{DampCoeff} \approx (0.65 \dots 0.75) \cdot 2 \sqrt{\text{SpringCoeff} \cdot m_{\text{corner}}}$$
3. **Отсутствие отдельного стабилизатора поперечной устойчивости (ARB):** всю угловую жёсткость крена создают сами пружины `Suspension1Spring`. Для высоких автомобилей (минивэнов, кроссоверов) рекомендуется брать собственную частоту подвески на `10–12%` выше чисто вертикальной нормы и ход `Length = 0.20..0.22` м, чтобы внутреннее колесо сохраняло запас отбоя `~14–15 см` и не отрывалось от дороги в поворотах.

| Ключ INI | Тип | По умолч. | Ед. изм. | Описание и назначение |
| --- | --- | :-: | :-: | --- |
| `BodyName` | строка | `"body"` | имя меша | Имя физического меша кузова в Blueprint (`Body`). |
| `Mount` | вектор (3) | *обяз.* | м `(X; Y; Z)` | Координаты центра колеса при **полном сжатии** подвески (`SpringSag = 1.0`) в СК кузова. |
| `Length` | число | `0.25` | м | Полный рабочий ход подвески от верхнего отбойника до полного отбоя. |
| `SpringCoeff` | число | `50000.0` | Н/м | Базовая жёсткость пружины. |
| `PreloadForce` | число | `5000.0` | Н | Сила предварительного поджатия пружины. |
| `PreloadRampBuffer` | число | `0.02` | м | Начальный участок сжатия `[0..PreloadRampBuffer]` плавного нарастания `PreloadForce`. |
| `DampCoeff` | число | `4000.0` | $\text{Н}\cdot\text{с/м}$ | Коэффициент демпфирования амортизатора на ходе **отбоя** (на сжатии делится на `3`). |
| `ProgressionCurve` | путь CSV | `"progression.csv"` | 1D-таблица | Кривая прогрессивности пружины (`Common/Vehicles/MotorCars/Tables/SpringProgression.csv`). |
| `NormalSag` | число | `0.33` | доля `[0..1]` | Номинальная статическая просадка подвески стоящего автомобиля (нулевая точка Bump Steer). |
| `SagToCamberCurve` | путь CSV | `""` | 1D-таблица | Кривая развала от сжатия (`Common/Vehicles/MotorCars/Tables/WheelSagToCamberCurve.csv`). |
| `SagToCamberCoeff` | число | `0.0` | град | Масштабный множитель кривой развала от просадки (противоположные знаки слева и справа). |
| `SteerToCamberCurve` | путь CSV | `""` | 1D-таблица | Кривая развала от поворота руля (`Common/Vehicles/MotorCars/Tables/WheelSteerToCamberCurve.csv`). |
| `SteerToCamberCoeff` | число | `0.0` | град | Масштабный множитель кривой развала от поворота колеса. |
| `SagToSteeringAngle` | число | `0.0` | град | Базовый угол кинематического подруливания при ходе сжатия/отбоя. |
| `SagToSteeringLinCoeff` | число | `0.5` | безразм. | Линейный коэффициент полинома подруливания от просадки подвески. |
| `SagToSteeringCubeCoeff` | число | `0.5` | безразм. | Кубический коэффициент полинома подруливания от просадки подвески. |
| `LonForceToSteeringCoeff` | число | `1.75` | безразм. | Чувствительность эластокинематического подруливания от продольной силы. |
| `LonForceToSteeringMaxAngle` | число | `0.0` | град | Максимальный угол подруливания от продольной силы колеса (тяга/торможение). |
| `LatForceToSteeringCoeff` | число | `1.75` | безразм. | Чувствительность эластокинематического подруливания от боковой силы. |
| `LatForceToSteeringMaxAngle` | число | `0.0` | град | Максимальный угол подруливания от боковой силы колеса в повороте. |

- **Выходы (`OutputElements`):** `SpringSag` (`Real`, `[0.0..1.0]` — текущая относительная просадка пружины), `WheelSteeringAngle` (`Real`, град — базовый угол поворота колеса).

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

### 1.2. `Suspension1Constant` — жёсткая подвеска постоянной нагрузки

Упрощённая нерейкастовая подвеска с нулевым ходом сжатия и фиксированной вертикальной нагрузкой (для рельсового транспорта). Связывается с одним колесом (`<Link Holder="Suspension..." Reference="Wheel..." />`) и передаёт на кузов только продольное усилие.

- **Параметры `.ini`:** `BodyName` (`"body"`), `Mount` (обязательный вектор `X; Y; Z` в м), `Load` (постоянная вертикальная нагрузка в Н, по умолчанию `100.0`).

### 1.3. `Steering2Ackermann` — рулевая трапеция Аккермана

В стандартных схемах CCD 2.0 объявляется под именем **`[Steering]`** и связывается с двумя управляемыми стойками (`SuspensionFrontLeft`, `SuspensionFrontRight`).

- Ширина колеи управляемой оси (`TracksWidth`, $W_{\text{track}}$) вычисляется автоматически по разности поперечных координат `Mount.Y` левой и правой стоек подвески.
- Внутреннее к повороту колесо поворачивается на полный угол $\alpha_{\text{inner}} = \text{MaxAngle} \cdot |\text{Steering}|$, а внешнее — на уменьшенный угол по геометрии Аккермана:
   $$\alpha_{\text{outer}} = \arctan\left(\frac{\text{InteraxleDistance}}{W_{\text{track}} + \dfrac{\text{InteraxleDistance}}{\tan(\alpha_{\text{inner}})}}\right)$$

| Ключ INI | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | :-: | :-: | --- |
| `MaxAngle` | число | `40.0` | градусы | Максимальный угол поворота **внутреннего** колеса при крайнем положении руля (`\|Steering\| = 1.0`). |
| `InteraxleDistance` | число | `1.0` | м | Продольная колёсная база автомобиля — точная разность $X_{\text{front}} - X_{\text{back}}$ между `Mount.X` передней и задней осей! |
| `BackSteering` | логич. | `false` | `true`/`false` | Если `true`, ось работает как задняя подруливающая (поворачивает колёса в противофазе). |
| `MinPositionInput` / `MaxPositionInput` | число | `-1.0` / `1.0` | ед. входа | Границы входного сигнала `Steering` для крайнего левого и правого положений руля. |

- **Входы (`InputProcessors`):** `Steering` (`Real`, `[-1.0..+1.0]`, `> 0` — вправо), `BrokenSteering` (`Boolean` — поломка рулевого).
- **Выходы (`OutputElements`):** `MaxSteeringAngle` (`Real`, рад — средний максимальный угол поворота управляемых колёс с учётом редукции Аккермана).

### 1.4. `SteeringSimple` — симметричное рулевое управление

Поворачивает колёса привязанной подвески (`<Link Holder="Steering..." Reference="Suspension..." />`) на одинаковый угол $\alpha = \text{MaxAngle} \cdot \text{Steering}$ без геометрии Аккермана и без поддержки `PowerSteering`.

- **Параметры `.ini`:** `MaxAngle` (градусы, по умолчанию `40.0`). **Входы:** `Steering` (`Real`, `[-1.0..+1.0]`), `BrokenSteering` (`Boolean`).

---

## 2. Контроллеры управления (`Input`)

### <a id="21-fuelcontrollercomputer-idler-и-fuelcontrollermechanical"></a>2.1. `FuelControllerComputer` (секция `[Idler]`) и `FuelControllerMechanical`

Управляют открытием дроссельной заслонки и цикловой подачей топлива в двигатель (`Engine`).

- **`FuelControllerComputer`** (во всех стандартных схемах CCD 2.0 имеет имя **`Idler`**) — ЭБУ инжекторного или дизельного ДВС. Требует обязательных связей `<Link>` с `Engine` и `Starter`, а также опциональной связи со `Speedometer` (для электронного ограничителя скорости `SpeedLimit`).
- **`FuelControllerMechanical`** — механический карбюратор с ручным управлением воздушной заслонкой (`Choke`). Требует связи `<Link>` с `Engine`.

> [!WARNING]
> **Две важные особенности симуляции `FuelControllerComputer` (`[Idler]`):**
>
> 1. **Обогащение смеси на холодном пуске (`+10%` момента ниже `30 °C`):** В диапазоне температур ОЖ `[LowTemperature..WorkingTemperature]` (по умолчанию `30..35 °C`) ЭБУ интерполирует обороты ХХ от `WarmupRpm` к `IdleRpm` и удерживает обогащённый состав смеси `Lambda = 0.8`. Поэтому холодный мотор сразу после спавна развивает примерно на **`10%` больше крутящего момента**, чем после прогрева выше `35 °C`.
> 2. **Разомкнутый регулятор холостого хода («анти-столл горб»):** Удержание `IdleRpm` рассчитывается только под компенсацию собственного трения ДВС $F(\text{IdleRpm})$ без обратной связи по внешней нагрузке на валу (и без добавления газа при просадке оборотов ниже `IdleRpm`). Когда в режиме `D` на тормозе насосное колесо гидротрансформатора нагружает коленвал на `45..60 Н·м`, обороты проседают. Чтобы мотор не глох и держал ровный ХХ в `D`, в таблице `Engine.csv` в строках `5%` и `10%` на колонке `500 RPM` формируется крутой «анти-столл горб» момента (подробно см. в [разделе 2.2 документа 03](./03_Components_TorqueTrain.md#22-критическое-правило-расчёта-enginecsv-powercurves-и-холостого-хода-в-d) и [разделе 5.3 документа 01](./01_Architecture_and_Config.md#53-точный-формат-таблицы-двигателя-enginecsv-powercurves)).

| Ключ INI | Класс контроллера | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | --- | :-: | :-: | --- |
| `IdleRpm` | `FuelControllerComputer` | число | `750.0` | об/мин | Целевые обороты холостого хода полностью прогретого двигателя. |
| `WarmupRpm` | `FuelControllerComputer` | число | `1000.0` | об/мин | Повышенные прогревочные обороты ХХ на холодном двигателе. |
| `LowTemperature` | `FuelControllerComputer` | число | `30.0` | °C | Нижняя граница температурного диапазона прогрева, ниже которой действуют полные `WarmupRpm` и `Lambda = 0.8`. |
| `WorkingTemperature` | `FuelControllerComputer` | число | `35.0` | °C | Верхняя граница прогрева ОЖ (полный переход к рабочим `IdleRpm` и табличной `Lambda`). |
| `SpeedLimit` | `FuelControllerComputer` | число | `0.0` | км/ч | Ограничитель максимальной скорости (`> 0` при связи со `Speedometer`; плавно урезает газ в коридоре $\pm 0.9\text{ км/ч}$). |
| `Throttle` | `FuelControllerComputer` | массив | `0; 0.1; 0.6; 0.7` | доля `[0..1]` | Узлы положения педали газа для интерполяции коэффициента избытка воздуха `Lambda`. |
| `Lambda` | `FuelControllerComputer` | массив | `1; 1.07; 1.02; 0.81` | безразм. | Коэффициент избытка воздуха $\lambda$ в точках `Throttle` (размеры массивов обязаны совпадать!). |
| `ThrottleIdle` | `FuelControllerMechanical` | число | `0.15` | доля `[0..1]` | Минимальное открытие заслонки карбюратора на холостом ходу. |
| `ThrottleChoke` | `FuelControllerMechanical` | число | `0.30` | доля `[0..1]` | Открытие заслонки при полностью вытянутом подсосе (`Choke = 1.0`). |
| `ThrottleIdleMaxRpm` | `FuelControllerMechanical` | число | `1500.0` | об/мин | Обороты ДВС, выше которых упор холостого хода `ThrottleIdle` отключается. |

- **Входы (`InputProcessors`):** `Accelerator` (`Real`, `[0.0..1.0]`), `FuelFeed` (`Boolean` — подаётся от `IgnitionKey`), `Choke` (`Real`, `[0.0..1.0]`, только `FuelControllerMechanical`).

### 2.2. `AbsTcs` — блок систем ABS, TCS и регулятора тормозных сил EBD

В стандартных схемах CCD 2.0 объявляется под именем **`[AbsTcs]`** строго **перед** `Idler`, питается от цепи зажигания (`IgnitionElectricCircuit`) и связывается с `Idler` и четырьмя тормозами (`BrakeFrontLeft`, `BrakeFrontRight`, `BrakeBackLeft`, `BrakeBackRight`).

- **Регулятор тормозных сил (EBD):** автоматически распределяет запрошенный тормозной момент между осями пропорционально текущей вертикальной нагрузке каждого колеса (с нижним порогом `5%` от номинальной нагрузки для вывешенных колёс).
- **ABS и TCS:** при напряжении выше `MinVoltage` удерживают продольное скольжение каждого колеса в оптимальном коридоре (`OptimalSlip` / `MaxSlip` / `SlipRange` из кривой продольного трения шины) и транслируют сигнал `Accelerator` в `Idler`.

| Ключ INI | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | :-: | :-: | --- |
| `AbsEquipped` / `TcsEquipped` | логич. | `true` / `true` | `true`/`false` | Оборудован ли автомобиль системами ABS и TCS (ASR). |
| `AbsIsOnByDefault` | логич. | `true` | `true`/`false` | **Внимание:** этот единый ключ включает по умолчанию при подаче питания **одновременно и ABS, и TCS**! |
| `Resistance` | число | `5.0` | Ом | Электрическое сопротивление блока в цепи зажигания. |
| `MinVoltage` / `NormalVoltage` | число | `9.0` / `12.0` | В | Минимальное и номинальное рабочее напряжение бортсети для работы ABS и TCS. |

- **Входы (`InputProcessors`):** `Brake` (`Real`, `[0..1]`), `Accelerator` (`Real`, `[0..1]`), `EnableABS` (`Boolean`), `EnableTCS` (`Boolean`), `Enabled` (`Boolean`).
- **Выходы (`OutputElements`):** `AbsActive` (`Boolean`), `TcsActive` (`Boolean`), `AbsEnabled` (`Boolean`), `TcsEnabled` (`Boolean`), `Enabled` (`Boolean`), `Intensity` (`Real`).

### 2.3. `ClutchInputProcessor` — нелинейный контроллер педали сцепления

В схемах с МКПП (`*Manual.xml`) объявляется под именем **`[ClutchPedal]`** (`<Link Holder="ClutchPedal" Reference="Clutch" />`).

- **Инверсия входа `Clutch`:** `0.0` = педаль **полностью нажата** (сцепление разомкнуто), `1.0` = педаль **полностью отпущена** (сцепление замкнуто). Закон смыкания дисков: $\text{Position} = (\text{Clutch})^{\text{Power}}$ (при `Power = 4.0` зона плавного схватывания лежит на `0.70..0.85`).
- **Параметры `.ini`:** `Power` (число, по умолчанию `4.0`).

### <a id="24-gearboxautomaticlogic-gearboxlogic-и-gearboxautomaticlogicbus"></a>2.4. `GearBoxAutomaticLogic` (секция `[GearBoxLogic]`) и `GearBoxAutomaticLogicBus`

Контроллеры селектора АКПП (`<Link Holder="GearBoxLogic" Reference="GearBoxAutomatic" />`), управляющие режимами `0 = P`, `1 = R`, `2 = N`, `3 = D` и ограничением верхней передачи `AutoShifterMaxGear` (`MaxGear`).

- **`GearBoxAutomaticLogic`** (легковой селектор в `*Auto.xml`): блокирует выход из паркинга `P (0)` без нажатия педали тормоза (`Brake >= ShiftMinBrake`) и при `ConsiderRpmWhenChangeState = true` защищает КПП от включения `P`, `R` или `D` при превышении пороговых оборотов выходного вала (`MaxParkingOutputRpm`, `MaxReverseOutputRpm`, `MaxDriveOutputRpm`), временно удерживая нейтраль `N (2)`.
- **`GearBoxAutomaticLogicBus`** (кнопочный селектор автобуса `D / N / R`): запрещает прямой переход `D <-> R` в обход `N`, поддерживает блокировку заднего хода `ReverseLock` и автоматически включает скрытую нейтраль на полной остановке с нажатым тормозом (`Brake > 0.1`).

| Ключ INI | Класс | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | --- | :-: | :-: | --- |
| `HasParkingState` | Оба | логич. | `true` | `true`/`false` | Наличие режима паркинга `P` (`0`). Для автобусов без паркинга ставится `false`. |
| `ShiftMinBrake` | `GearBoxAutomaticLogic` | число | `0.1` | доля `[0..1]` | Минимальное нажатие педали тормоза для вывода селектора из положения `P`. |
| `ConsiderRpmWhenChangeState` | `GearBoxAutomaticLogic` | логич. | `true` | `true`/`false` | Включает защиту от переключения в `P`, `R`, `D` при быстром вращении выходного вала КПП. |
| `MaxParkingOutputRpm` | `GearBoxAutomaticLogic` | число | `50.0` | об/мин | Максимальные обороты выходного вала КПП, при которых разрешено включение `P`. |
| `MaxReverseOutputRpm` | `GearBoxAutomaticLogic` | число | `50.0` | об/мин | Максимальные обороты выходного вала при качении вперёд для включения `R`. |
| `MaxDriveOutputRpm` | `GearBoxAutomaticLogic` | число | `50.0` | об/мин | Максимальные обратные обороты выходного вала при качении назад для включения `D`. |
| `HasReverseLock` | `GearBoxAutomaticLogicBus` | логич. | `false` | `true`/`false` | Требуется ли сигнал разблокировки (`ReverseLock = false`) для включения заднего хода `R`. |

- **Входы:** `AutoShifterState` (`Integer`: `0=P`, `1=R`, `2=N`, `3=D`), `AutoShifterMaxGear` (`Integer`), `Brake` (`Real`), `Accelerator` (`Real`), `ReverseLock` (`Boolean`). **Состояние:** `AutoShifterState` (`InternalName="State"`, `Integer`).
- **Выходы:** `AutoShifterState`, `CurrentState`, `AutoShifterMaxGear`, `AutoShifterCurrentGear`, `AutoShifterTargetGear` (все — `Integer`).

### 2.5. `IgnitionKeyProcessor` — контроллер замка зажигания

В стандартных схемах объявляется под именем **`[IgnitionKey]`** (секция в `Vehicle.ini` параметров не имеет) и принимает вход **`Ignition`** (`Integer`: `0 = Off`, `1 = Ignition`, `2 = Starter`).

- **Связи `<Link Holder="IgnitionKey" Reference="..." />`:**
   1. `IgnitionElectricCircuit` — замыкает цепь зажигания при `Ignition != 0`.
   2. `Starter` и `StarterRelay` — включает стартер и его реле при `Ignition == 2`.
   3. `Idler` (`FuelControllerComputer`) — передаёт `FuelFeed = (Ignition != 0)`. **Критично для дизеля (`EngineDiesel`):** именно эта связь перекрывает подачу топлива и глушит дизельный двигатель при повороте ключа в `0`!
   4. `GearBoxLogic` (`GearBoxAutomaticLogic`) — **блокировка стартера на передаче**: если селектор АКПП стоит в `D (3)` или `R (1)`, положение ключа `2 (Starter)` принудительно понижается до `1 (Ignition)`.

---

## 3. Телеметрия и обратная связь на руле (`Output`)

### 3.1. `PowerSteering` — усилитель руля (ГУР/ЭУР) и Force Feedback

В стандартных схемах объявляется под именем **`[PowerSteering]`** (`<Link>` со `Steering` и `Engine`). Рассчитывает силовую обратную связь (FFB) на рулевом валу от стабилизирующего момента и боковых сил шин, снижая усилие на руле по мере роста оборотов ДВС от `ActivationRpm` до `FullPowerRpm`.

| Ключ INI | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | :-: | :-: | --- |
| `SteeringRatio` | число | `17.0` | безразм. | Передаточное число рулевого механизма (отношение угла поворота руля к углу поворота колёс). |
| `ActivationRpm` / `FullPowerRpm` | число | `0.0` / `0.1` | об/мин | Обороты ДВС начала работы и выхода ГУР/ЭУР на полную мощность (обычно `FullPowerRpm = 500`). |
| `MaxTorque` | число | `1.0` | $\text{Н}\cdot\text{м}$ | Максимальный момент помощи усилителя на рулевом валу (обычно `35..45`). |
| `TorqueThreshold` | число | `1.0` | $\text{Н}\cdot\text{м}$ | Порог момента на руле, ниже которого усилитель не гасит усилие (сохраняет околонулевую информативность, `1.5..2.5`). |
| `ConstFriction` | число | `0.0` | $\text{Н}\cdot\text{м}$ | Постоянный момент сухого трения в рулевой колонке. |
| `DampingCoeff` | число | `0.0` | $\text{Н}\cdot\text{м}\cdot\text{с/рад}$ | Коэффициент вязкого демпфирования вращения рулевого колеса для FFB. |
| `MechanicalTrailCoeff` | число | `0.0` | м | Плечо механического кастора: добавляет возвращающий момент $-F_{\text{lat}} \cdot \text{MechanicalTrailCoeff}$ (реакцию на увод и боковые толчки, `0.002..0.025`). |

- **Входы:** `BrokenPowerSteering` (`InternalName="Broken"`, `Boolean`).
- **Выходы:** `ForceFeedbackTorque` (`InternalName="Torque"`), `ForceFeedbackFriction` (`InternalName="Friction"`), `ForceFeedbackDamping` (`InternalName="Damping"`), `ForceFeedbackSlipAngle` (`InternalName="SlipAngle"`), `ForceFeedbackInertia` (`InternalName="Inertia"`).

### 3.2. `Speedometer` — спидометр и одометр

В стандартных схемах объявляется под именем **`[Speedometer]`** и связывается с главным дифференциалом (`DifferentialAxle` в схемах `Awd*` или `Differential` в схемах `Fwd*`/`Rwd*`).

- **Точная формула `MetersPerRotation`:** путь за один оборот карданного вала равен длине окружности колеса, **разделённой** на передаточное число главной пары `GearRatio` того дифференциала, к которому привязан спидометр:
   $$\text{MetersPerRotation} = \frac{2\pi R_{\text{wheel}}}{i_{\text{diff}}}$$
   *(При разных главных парах МКПП и АКПП задаётся в секциях `[Speedometer.TransmissionType.Manual]` и `[Speedometer.TransmissionType.Auto]`).*
- **Входы:** `MileageMultiplier` (`Real`, `1.0`). **Состояния:** `Mileage` (м), `SessionMileage` (м). **Выходы:** `Speed` (м/с), `Mileage` (м), `SessionMileage` (м).

### 3.3. `FuelConsumptionMeter` — датчик среднего расхода топлива

Маршрутный компьютер скользящего и сессионного среднего расхода топлива (`<Link>` со `Speedometer` и `Engine`).

- **Параметры `.ini`:** `IsEquipped` (`Boolean`, `false`), `AveragingDistance` (км, `1.0`), `AveragingStep` (м, `100.0`).
- **Состояния:** `AverageFuelConsumption` (л/м), `SessionMileage` (м), `SessionFuelConsumed` (л), `IsEnabled` (`Boolean`). **Выходы:** `AverageFuelConsumption` (л/м), `SessionAverageFuelConsumption` (л/м; для перевода в л/100 км умножается на `100000`).

---

## 4. Наддув, пневматика, баки, аэродинамика и внешние силы

### 4.1. `Turbocharger` — турбокомпрессор наддува ДВС

Повышает давление воздуха на впуске ДВС (`<Link Holder="Turbocharger" Reference="Engine" />`, вход `<Processor Input="Accelerator" Component="Turbocharger" InternalName="Throttle" />`), пропорционально умножая индикаторный момент сгорания.

| Ключ INI | Тип | По умолч. | Ед. изм. | Описание |
| --- | --- | :-: | :-: | --- |
| `MinRpm` | число | `1500.0` | об/мин | Минимальные эффективные обороты ($\text{RPM} \cdot \text{Throttle}$), с которых начинается наддув ($\text{ratio} > 1.0$). |
| `RefRpm` | число | `6000.0` | об/мин | Опорные обороты (`> MinRpm`), на которых при полном газе достигается `RefCompressionRatio`. |
| `RefCompressionRatio` | число | `1.6` | безразм. | Степень повышения давления во впуске на оборотах `RefRpm` при `Throttle = 1.0`. |
| `LagTime` | число | `1.0` | с | Параметр инерционности турбины (рекомендуется **`0.08..0.10`**, см. правило №1 ниже!). |

> [!CAUTION]
> **Два критических правила расчёта `Turbocharger` и таблицы `Engine.csv`:**
>
> 1. **Четырёхкратный множитель времени раскрутки ($\tau_{\text{eff}} = 4 \cdot \text{LagTime}$):** рабочие обороты турбины сглаживаются со скоростью $\frac{0.25}{\text{LagTime}} \cdot dt$, поэтому истинная постоянная времени равна **$\tau_{\text{eff}} = 4 \cdot \text{LagTime}$**, а на холостом ходу (`Throttle = 0`) обороты турбины падают до `0`. Чтобы избежать полуторасекундной мёртвой зоны без наддува на старте и получить реальный отклик `0.32..0.40 с`, ставьте **`LagTime = 0.08..0.10`**.
> 2. **Неограниченный рост выше `RefRpm` и 2D-деление `Engine.csv` по `(RPM, Throttle)`:** формула коэффициента наддува:
>    $$\text{ratio}(\text{RPM}, \text{Throttle}) = 1 + (\text{RefCompressionRatio} - 1)\sqrt{\frac{\max(0,\; \text{RPM} \cdot \text{Throttle} - \text{MinRpm})}{\text{RefRpm} - \text{MinRpm}}}$$
>    **не имеет верхнего предела** выше `RefRpm` и зависит от `Throttle`! Каждую ячейку сгорания `(thr, RPM)` в `Engine.csv` необходимо делить на **её собственный** $\text{ratio}(\text{RPM}, \text{thr})$ при данном `thr = row_pct / 100.0`, не трогая последнюю строку механических потерь $F(\text{RPM})$.

### <a id="42-пневмосистема-receiver-valvemulticircuit-actuatorpneumatic12chamber"></a>4.2. Пневмосистема: `Receiver`, `ValveMultiCircuit`, `ActuatorPneumatic1Chamber`, `ActuatorPneumatic2Chamber`

Компоненты пневмотормозов и приводов дверей автобусов/грузовиков (давления задаются в **абсолютных барах**, где атмосфера $\approx 1.0\text{ бар}$):

- **`Receiver` (ресивер)** и **`ValveMultiCircuit` (многоконтурный защитный клапан):**
   Порядок объявления связей `<Link Holder="ValveMultiCircuit" Reference="Receiver..." />` в XML-схеме **критичен** — он определяет индекс контура `i`, напрямую сопоставляемый с `OpenPressure[i]` и `ClosePressure[i]` в `Vehicle.ini`!
- **`ActuatorPneumatic1Chamber` (однокамерный привод)** и **`ActuatorPneumatic2Chamber` (двухкамерный цилиндр дверей):**
   Связываются с питающим ресивером (`<Link Holder="Actuator..." Reference="Receiver..." />`) и принимают вход `Position` (`Real`, `[0.0..1.0]`).

| Ключ INI | Компоненты | По умолч. | Ед. изм. | Описание |
| --- | --- | :-: | :-: | --- |
| `Volume` | `Receiver`, `ValveMultiCircuit` | `1.0` | л | Геометрический объём воздушного ресивера / полости клапана. |
| `MaxPressure` / `WorkPressure` | Все 4 компонента | `8.0` / `8.0` (`6.0`) | бар (абс.) | Давление открытия (`MaxPressure - 0.05`) и закрытия (`WorkPressure + 0.05`) клапана сброса. |
| `BlowOffDiameter` / `InitialPressure` | Все 4 компонента | `5.0` / атм. | мм / бар | Диаметр предохранительного клапана сброса и начальное абсолютное давление воздуха. |
| `PressureDelta` | `ValveMultiCircuit`, `Actuator*` | `1.0` | бар | Перепад давлений плавного открытия перепускного клапана. |
| `ValveDiameter` / `AirLossDiameter` | `ValveMultiCircuit` | `10.0` / `1.0` | мм | Диаметр клапана каждого контура и диаметр естественной утечки воздуха во впуске. |
| `OpenPressure` / `ClosePressure` | `ValveMultiCircuit` | *пусто* | бар | Массивы пороговых перепадов прямой накачки и обратного перепуска контуров (`P1; P2; ...`). |
| `MinVolume` / `MaxVolume` | `ActuatorPneumatic1/2Chamber` | `1.0` / `2.0` | л | Минимальный (сжатый) и максимальный (разжатый) объёмы рабочей камеры. |
| `MinVolumePressure` / `MaxVolumePressure` | `ActuatorPneumatic1/2Chamber` | `4.0` / `7.0` | бар (абс.) | Целевые давления в камере при `Position = 0.0` и `Position = 1.0`. |
| `FeedDiameter` / `LeakageDiameter` | `ActuatorPneumatic1/2Chamber` | `10.0` / `5.0` | мм | Диаметры перепускного клапана накачки и отверстия аварийной утечки. |
| `PistonMass` | `ActuatorPneumatic2Chamber` | `0.0` (**`> 0`!**) | кг | Масса поршня и створки двери. **Обязательно задать `> 0`, иначе произойдёт деление на ноль!** |
| `PistonFriction` / `PistonDiameter` | `ActuatorPneumatic2Chamber` | `0.0` / `1.0` | Н / м | Сила сухого трения страгивания и диаметр поршня (`> 0`). |
| `PistonInitialPosition` | `ActuatorPneumatic2Chamber` | `0.5` | доля `[0..1]` | Начальное положение поршня (`0.0` — крайнее левое, `1.0` — крайнее правое). |

- **Выходы:** у `Receiver` — `Pressure` (бар); у `ActuatorPneumatic1Chamber` — `WorkPressure`, `MaxVolumePressure`; у `ActuatorPneumatic2Chamber` — `WorkPressure`, `MaxVolumePressure`, `Position` (`[0..1]`), `PressureLeft`, `PressureRight`.

### 4.3. `Tank` — топливный и масляный бак

Используется для топливного бака **`[FuelTank]`** и масляного бака АКПП **`[OilTank]`** (в схемах `*Auto.xml`).

- **Параметры `.ini`:** `Volume` (ёмкость в л, по умолчанию `1.0`, должна быть `> 0`), `StartLevel` (начальная заполненность `[0.0..1.0]`, по умолчанию `1.0`).
- **Входы:** `FuelLeak` (`InternalName="DrainSpeed"`, л/с), `UnlimitedFuelTank` (`InternalName="EnableUnlimited"`, `Boolean`). **Состояние:** `FuelVolume` (`InternalName="LiquidVolume"`, л). **Выходы:** `FuelVolume` (`LiquidVolume`, л), `FuelVolumeMax` (`Volume`, л), `FuelRelative` (`LiquidRelativeVolume`, `[0..1]`).

### 4.4. `AerodynamicsCarSimple` — аэродинамика кузова

В стандартных схемах объявляется под именем **`[Aerodynamics]`**. Прикладывает к центру масс кузова (`BodyName = Body`) силу лобового сопротивления $F_{\text{drag}} = \frac{1}{2}\rho_{\text{air}} C_d A v^2$ ($\rho_{\text{air}} = 1.225\text{ кг/м}^3$) и вертикальную подъёмную/прижимную силу $F_{\text{lift}} = \frac{1}{2}\rho_{\text{air}} C_l A v_{\text{fwd}}^2$.

- **Параметры `.ini`:** `BodyName` (`Body`), `DragCoeff` ($C_d$, по умолчанию `0.4`), `LiftCoeff` ($C_l$, по умолчанию `-0.3`; `< 0` — прижимная сила), `FrontalArea` (площадь миделя в $\text{м}^2$, по умолчанию `2.0`).

### 4.5. `ExternalThruster` — внешний линейный толкатель

Прикладывает управляемую силу в локальной точке кузова `Pivot` вдоль вектора `Direction` с линейным спадом тяги до нуля между скоростями `MaxForceVelocity` и `ZeroForceVelocity`.

- **Параметры `.ini`:** `BodyName` (`"body"`), `Pivot` (обязательный вектор `X; Y; Z` в м), `Direction` (обязательный вектор `X; Y; Z`), `MaxForce` (Н, `100.0`), `MaxForceVelocity` (м/с), `ZeroForceVelocity` (м/с), `MinPower` (`-1.0`), `MaxPower` (`1.0`). **Вход:** `Power` (`Real` или `Boolean`).
