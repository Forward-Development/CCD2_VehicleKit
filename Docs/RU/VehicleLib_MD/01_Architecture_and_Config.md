# 01. Конфигурационные файлы, схемы и таблицы в City Car Driving 2.0

**Навигация:** [README](./README.md) | [00. Быстрый старт](./00_Quick_Start.md) | **01. Архитектура и конфиги** | [02. Именованный ввод/вывод](./02_Named_IO_and_States.md) | [03. Трансмиссия](./03_Components_TorqueTrain.md) | [04. Электрика](./04_Components_Electrics.md) | [05. Шасси и подсистемы](./05_Components_Chassis_and_Subsystems.md)

---

## 1. Файлы мода и разрешение путей в City Car Driving 2.0

В проекте **City Car Driving 2.0** ассеты и конфигурационные файлы модифицированного автомобиля располагаются в папке плагина мода:
 `Plugins/Mods/Content/<ИмяМода>/` (например, `Plugins/Mods/Content/CieBensVGruppe/`).

Набор файлов в папке мода включает:

- **`V_<ИмяМода>.uasset`** — Blueprint-прототип автомобиля (содержит 3D-меши кузова `Body`, колёс `Wheel*`, подвесок `Suspension*`, фар, приборной панели и настройки их физики/контроллеров).
- **`DA_<ИмяМода>.uasset`** — карточка автомобиля для меню выбора транспорта (связывает комплектации в UI с опциями из `options.xml`).
- **`ST_<ИмяМода>.uasset`** — таблица строк локализации (`StringTable`).
- **`Vehicle.ini`** — главный конфигурационный файл параметров всех физических узлов.
- **`options.xml`** — описание комплектаций (например, выбор АКПП/МКПП и переключение XML-схемы).
- **`Engine.csv`** — таблица крутящего момента и механических потерь двигателя.
- *(Опционально)* собственные файлы XML-схем (`.xml`) и таблиц кривых (`.csv`).

### Правила указания путей в `Vehicle.ini` и `options.xml`

Все пути к файлам схем (`.xml`) и таблиц кривых (`.csv`), записанные внутри `Vehicle.ini` и `options.xml`, отсчитываются **относительно корневой директории `Content/` основного проекта игры City Car Driving 2.0**:

1. **Обращение к стандартным общим ресурсам игры (`Content/Common/Vehicles/MotorCars/...`):**
   Записывается напрямую без начального слэша:
   ```ini
   DefaultSchema = Common/Vehicles/MotorCars/Schemes/AwdAuto.xml
   ProgressionCurve = Common/Vehicles/MotorCars/Tables/SpringProgression.csv
   ForceCurveLateral = Common/Vehicles/MotorCars/Tables/WheelForceLat.csv
   ```
2. **Обращение к собственным файлам внутри папки мода (`Plugins/Mods/Content/<ИмяМода>/...`):**
   Чтобы выйти из папки `Content/` основного проекта в папку плагина `Plugins/Mods/Content/<ИмяМода>/`, используйте относительный путь `../Plugins/Mods/Content/<ИмяМода>/...`:
   ```ini
   PowerCurves = ../Plugins/Mods/Content/CieBensVGruppe/Engine.csv
   ```

> [!NOTE]
> При создании нового мода убедитесь, что в секции `[Engine]` параметр `PowerCurves` указывает на файл `Engine.csv` в папке вашего мода (`../Plugins/Mods/Content/<ИмяМода>/Engine.csv`).

---

## 2. Синтаксис `Vehicle.ini` и система опций `options.xml`

### 2.1. Секция `[Vehicle]` и секции компонентов `[ИмяКомпонента]`

Файл `Vehicle.ini` чувствителен к регистру символов (и в заголовках секций, и в именах параметров) и сохраняется в кодировке **UTF-8**.

- Системная секция **`[Vehicle]`** содержит только один параметр — **`DefaultSchema`** (путь к XML-схеме по умолчанию, если схема не переопределена опцией в `options.xml`):
   ```ini
   [Vehicle]
   DefaultSchema = Common/Vehicles/MotorCars/Schemes/AwdAuto.xml
   ```
- Каждая остальная секция **`[ИмяКомпонента]`** соответствует атрибуту `Name="..."` конкретного компонента из XML-схемы (например, `[Engine]`, `[Idler]`, `[GearBoxAutomatic]`, `[SuspensionFrontLeft]`).

> [!IMPORTANT]
> **Где задаются масса и центр тяжести автомобиля?**
>  В `Vehicle.ini` нет параметров массы и центра тяжести кузова — они настраиваются непосредственно на корневом физическом компоненте **`Body`** (`StaticMeshComponent`) внутри Blueprint автомобиля `V_<ИмяМода>.uasset` в редакторе Unreal Engine 5:
>
> 1. **Подрессоренная масса (`Override Mass`):** в разделе **Physics** компонента `Body` обязательно включите галочку **`Override Mass`** и укажите в поле **`Mass in Kg`** подрессоренную массу кузова (снаряжённую массу автомобиля за вычетом массы четырёх колёс: $M_{\text{body}} = M_{\text{curb}} - 4 \cdot M_{\text{wheel}}$). Если оставить `Override Mass` выключенным, UE5 рассчитает массу из объёма коллизии по нелинейной формуле $1000 \cdot V^{0.75}$, что занижает массу крупных автомобилей почти вдвое.
> 2. **Смещение центра масс (`Center Of Mass Offset` / `COMNudge`):** задаётся в разделе **Physics** в **сантиметрах** как относительное смещение $(\Delta X, \Delta Y, \Delta Z)$ от автоматически вычисленного геометрического центра объёма коллизионных оболочек кузова ($\text{COM}_{\text{final}} = \text{COM}_{\text{auto}} + \text{Center Of Mass Offset}$).
> 3. **Физический материал коллизии:** в ассете `StaticMesh` кузова `Body` должен быть назначен **`Simple Collision Physical Material`**, а на всех остальных компонентах и мешах в Blueprint (колёсах, подвесках, стёклах, деталях салона) должен стоять профиль коллизии **`NoCollision`**.

### 2.2. Система комплектаций `options.xml` и переопределение секций в `Vehicle.ini`

Файл `options.xml` в папке мода позволяет использовать один `Vehicle.ini` для нескольких комплектаций автомобиля (например, версий с АКПП и МКПП или с разными двигателями):

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<Options>
    <AvailableOptions>
        <!-- Только ОДНА опция в файле может иметь атрибут SetSchema="true" -->
        <Option Name="TransmissionType" SetSchema="true">
            <OptionValue Value="Auto"   SchemaName="Common/Vehicles/MotorCars/Schemes/AwdAuto.xml" />
            <OptionValue Value="Manual" SchemaName="Common/Vehicles/MotorCars/Schemes/AwdManual.xml" />
        </Option>

        <!-- Дополнительная опция двигателя (без смены XML-схемы) -->
        <Option Name="EngineType" SetSchema="false">
            <OptionValue Value="1.6TD" />
            <OptionValue Value="1.8I" />
        </Option>
    </AvailableOptions>

    <!-- Опциональный блок запрета несовместимых комбинаций -->
    <Compatibilities>
        <Compatibility FirstOptionType="EngineType" FirstOptionValue="1.6TD"
                       SecondOptionType="TransmissionType" SecondOptionValue="Manual"
                       Enable="false" />
    </Compatibilities>
</Options>
```

> [!TIP]
> **Как настроить сразу две коробки передач (АКПП/МКПП) и два типа двигателя (Бензин/Дизель):**
>  Поскольку атрибут `SetSchema="true"` в `options.xml` может быть только у **одной** опции, привяжите `SetSchema="true"` к выбору КПП (`TransmissionType`: `*Auto.xml` и `*Manual.xml`), а для двигателей используйте `Class="EngineGazoline"` из стандартных схем как универсальный базовый ДВС. Тип двигателя (бензиновый или дизельный) в этом случае полностью задаётся через секции `[Engine.EngineType.1.8I]` и `[Engine.EngineType.1.6TD]` в `Vehicle.ini` за счёт разных таблиц крутящего момента `PowerCurves`, рабочих оборотов `RpmLimit` и холостого хода `[Idler.EngineType.*]` (подробно см. в [разделе 2.1 документа 03](./03_Components_TorqueTrain.md#21-бензиновый-и-дизельный-двс-engine)).

Чтобы переопределить отдельные параметры компонента только для конкретной выбранной опции, добавьте в `Vehicle.ini` секцию вида **`[ИмяКомпонента.ИмяОпции.ЗначениеОпции]`**:

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

При загрузке автомобиля сначала читаются ключи из базовой секции `[ИмяКомпонента]`, а затем поверх них накладываются ключи из активных секций `[ИмяКомпонента.ИмяОпции.ЗначениеОпции]`.

### 2.3. Критические правила и ограничения синтаксиса `Vehicle.ini`

> [!CAUTION]
> **Два главных правила `.ini`, нарушение которых ломает загрузку физики:**
>
> 1. **Разделитель списков и 3D-векторов — строго точка с запятой `;` БЕЗ пробела перед `;`!**
>    Запятые `,` для разделения чисел в векторах (`Mount`) и массивах (`Gears`, `ShiftAccelerator`, `OptimalRpm`) **не поддерживаются**. При этом комбинация **«пробел перед `;`» (`" ;"`)** воспринимается парсером `.ini` как начало комментария до конца строки и обрезает остальные числа!
>    - **ПРАВИЛЬНО** (слитно перед `;`, пробел или табуляция *после* `;` разрешены):
>       ```ini
>       Mount = 1.63; 0.85; 0.4455
>       Gears = -3.416; 4.377; 2.859; 1.921; 1.368; 1.000; 0.820; 0.728
>       ```
>    - **ОШИБКА** (запятые или пробел перед `;`):
>       ```ini
>       Mount = 1.63, 0.85, 0.4455     ; ОШИБКА: запятые не парсятся!
>       Mount = 1.63 ; 0.85 ; 0.4455   ; ОШИБКА: пробел перед ';' отрежет 0.85 и 0.4455!
>       ```
> 2. **Лимиты длины заголовка секции и строки:**
>    - Длина имени секции внутри квадратных скобок `[...]` (включая `.ИмяОпции.ЗначениеОпции`) **не должна превышать 49 символов** (`50` байт с терминатором). Более длинные заголовки обрезаются, и опция не срабатывает.
>    - Длина одной строки в `Vehicle.ini` **не должна превышать 400 символов**.

| Тип параметра в `.ini` | Правила записи | Пример |
| --- | --- | --- |
| **Вещественное число (`Real`)** | Десятичная точка `.` (не запятая!). Допустима запись `1e-4`. | `SpringCoeff = 45000.0` |
| **Целое число (`Integer`)** | Целое число со знаком. | `StartingGear = -1` |
| **Логический флаг (`Boolean`)** | `true` / `false` (также `1` / `0`, `yes` / `no`, `on` / `off`). | `Locked = false` |
| **Строка (`String`)** | Путь или имя **без кавычек**. | `BodyName = Body` |
| **3D-вектор (`Vector3`)** | Ровно 3 числа через `;` без пробела перед `;`. | `Mount = -1.59; -0.85; 0.4589` |
| **Массив чисел** | Числа по порядку через `;` без пробела перед `;`. В парных таблицах (`ShiftAccelerator`, `OilPressureRPM`) аргументы должны идти строго по возрастанию. | `OilPressureRPM = 0; 750; 4600` |

---

## 3. Стандартные XML-схемы легковых автомобилей в City Car Driving 2.0

В директории `Common/Vehicles/MotorCars/Schemes/` расположены **6 готовых схем** для всех комбинаций типа привода и коробки передач:

- **Полный привод (AWD):** `AwdAuto.xml` (АКПП), `AwdManual.xml` (МКПП)
- **Передний привод (FWD):** `FwdAuto.xml` (АКПП), `FwdManual.xml` (МКПП)
- **Задний привод (RWD):** `RwdAuto.xml` (АКПП), `RwdManual.xml` (МКПП)

### 3.1. Полный перечень секций `[ИмяКомпонента]` в `Vehicle.ini` для стандартных схем

| Подсистема | Секции в `Vehicle.ini` (`[Name]`) | Класс (`Class` в XML) | В каких схемах присутствует |
| --- | --- | --- | --- |
| **Двигатель, топливо и охлаждение** | `[Engine]`<br>`[Idler]`<br>`[FuelTank]` | `EngineGazoline`<br>`FuelControllerComputer`<br>`Tank` | Во всех 6 схемах |
| **Замок зажигания и пуск (стартер)** | `[IgnitionKey]`<br>`[Starter]`<br>`[StarterRelay]`<br>`[StarterGear]`<br>`[StarterBendix]` | `IgnitionKeyProcessor`<br>`EngineElectric`<br>`Relay`<br>`Reducer`<br>`Bendix` | Во всех 6 схемах |
| **Бортовая электросеть и приборка** | `[Battery]`<br>`[Generator]`<br>`[BatteryElectricCircuit]`<br>`[GeneratorElectricCircuit]`<br>`[IgnitionElectricCircuit]`<br>`[CarElectrics]` | `Battery`<br>`Generator`<br>`ElectricCircuit`<br>`ElectricCircuit`<br>`ElectricCircuit`<br>`CarElectrics` | Во всех 6 схемах *(для цепей `[*ElectricCircuit]` секции в `.ini` не требуются)* |
| **Коробка передач: АКПП (`*Auto.xml`)** | **`[GearBoxAutomatic]`**<br>`[TorqueConverter]`<br>`[GearBoxLogic]`<br>`[GearBoxAutomaticPump]`<br>`[OilTank]` | `GearBoxAutomatic`<br>`HydraulicTorqueConverter`<br>`GearBoxAutomaticLogic`<br>`Pump`<br>`Tank` | **Только в `AwdAuto`, `FwdAuto`, `RwdAuto`** |
| **Коробка передач: МКПП (`*Manual.xml`)** | **`[GearBox]`**<br>`[Clutch]`<br>`[ClutchPedal]` | `GearBox`<br>`ClutchFriction`<br>`ClutchInputProcessor` | **Только в `AwdManual`, `FwdManual`, `RwdManual`** |
| **Дифференциалы: полный привод (`Awd*`)** | **`[DifferentialAxle]`** *(межосевой)*<br>**`[FrontAxisRelay]`** *(муфта пер. оси)*<br>**`[DifferentialWheelFront]`** *(передний)*<br>**`[DifferentialWheelBack]`** *(задний)* | `DifferentialOpen`<br>`Relay`<br>`DifferentialOpen`<br>`DifferentialOpen` | **Только в `AwdAuto` и `AwdManual`** *(спидометр подключён к `DifferentialAxle`)* |
| **Дифференциал: монопривод (`Fwd*`, `Rwd*`)** | **`[Differential]`** | `DifferentialOpen` | **Только в `FwdAuto`, `FwdManual`, `RwdAuto`, `RwdManual`** |
| **Подвеска (4 стойки)** | `[SuspensionFrontLeft]`<br>`[SuspensionFrontRight]`<br>**`[SuspensionBackLeft]`**<br>**`[SuspensionBackRight]`** | `Suspension1Spring` | Во всех 6 схемах |
| **Колёса / шины (4 шт.)** | `[WheelFrontLeft]`<br>`[WheelFrontRight]`<br>**`[WheelBackLeft]`**<br>**`[WheelBackRight]`** | `WheelRaycast` | Во всех 6 схемах |
| **Тормозные механизмы (4 шт.) + ABS** | `[BrakeFrontLeft]`<br>`[BrakeFrontRight]`<br>**`[BrakeBackLeft]`**<br>**`[BrakeBackRight]`**<br>`[AbsTcs]` | `BrakeFriction`<br>`BrakeFriction`<br>`BrakeFriction`<br>`BrakeFriction`<br>`AbsTcs` | Во всех 6 схемах |
| **Рулевое, ГУР/FFB, спидометр, кузов** | `[Steering]`<br>`[PowerSteering]`<br>`[Speedometer]`<br>`[Aerodynamics]` | `Steering2Ackermann`<br>`PowerSteering`<br>`Speedometer`<br>`AerodynamicsCarSimple` | Во всех 6 схемах |

> [!WARNING]
> **Четыре важные особенности именования секций и мешей в City Car Driving 2.0:**
>
> 1. **Задняя ось именуется через `Back`, а НЕ `Rear` (обязательно для Blueprint и стандартных схем):**
>    - В Blueprint прототипа (`V_<ИмяМода>`) и в XML-схеме визуальные и физические узлы колёс и ступиц **обязаны** называться `SuspensionFrontLeft/Right`, **`SuspensionBackLeft/Right`**, `WheelFrontLeft/Right`, **`WheelBackLeft/Right`** — по этим именам игра сливает компоненты мода с базовым шаблоном автомобиля и связывает 3D-меши колёс с физикой.
>    - В стандартных XML-схемах секции задних тормозов также названы **`[BrakeBackLeft]`** и **`[BrakeBackRight]`** (если в `Vehicle.ini` написать `Rear`, секции не совпадут с XML-схемой и останутся со значениями по умолчанию).
> 2. **Разные имена секций дифференциала в стандартных схемах AWD и FWD/RWD:**
>    В готовых схемах `AwdAuto.xml` и `AwdManual.xml` межосевой дифференциал назван **`[DifferentialAxle]`**, а межколёсные — `[DifferentialWheelFront]` и `[DifferentialWheelBack]`. В моноприводных же схемах (`Fwd*.xml`, `Rwd*.xml`) единственный дифференциал назван **`[Differential]`**. *(В собственной XML-схеме дифференциал можно назвать произвольно, главное — совпадение `Name` в XML и `[Name]` в `Vehicle.ini`).*
> 3. **Разные имена секций АКПП и МКПП в стандартных схемах:**
>    В стандартных схемах `*Auto.xml` коробка передач имеет имя **`[GearBoxAutomatic]`**, а в `*Manual.xml` — **`[GearBox]`**. *(В собственной XML-схеме имя секции определяется атрибутом `Name` компонента в XML).*
> 4. **Имя физического меша кузова — `Body` (`BodyName = Body`):**
>    Корневой физический `StaticMeshComponent` кузова в Blueprint шаблона автомобиля обязан называться **`Body`**. В секциях `[Suspension*]` и `[Aerodynamics]` параметр `BodyName` по умолчанию уже равен `"Body"`, поэтому при использовании меша `Body` указывать `BodyName = Body` рекомендуется для наглядности (но указывать любое другое имя, которого нет среди физических мешей/костей в Blueprint, нельзя).

---

## 4. Устройство XML-схемы `<Vehicle>` и создание собственной схемы мода

Любой XML-файл схемы автомобиля имеет корневой тег **`<Vehicle>`** и состоит из 6 разделов:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Vehicle>
    <!-- 1. Список компонентов: Name задаёт имя секции [Name] в Vehicle.ini, Class — тип узла -->
    <Components>
        <Component Name="Turbocharger" Class="Turbocharger" />
        <Component Name="Engine" Class="EngineDiesel" />
        <Component Name="Idler" Class="FuelControllerComputer" />
    </Components>
    <!-- 2. Направленные связи между компонентами (от ведущего/владельца Holder к ведомому Reference) -->
    <Links>
        <Link Holder="Turbocharger" Reference="Engine" />
        <Link Holder="IgnitionKey" Reference="Idler" />
        <Link Holder="BatteryElectricCircuit" Reference="IgnitionElectricCircuit" Connected="false" />
        <Link Holder="CarElectrics" Reference="GearBox" StartIconGearBox="true" />
    </Links>
    <!-- 3. Выходные каналы телеметрии и приборной панели -->
    <OutputElements>
        <Element Name="RPM" Type="Real" Component="Engine" />
        <Element Name="DashboardRPM" Type="Real" Component="CarElectrics" InternalName="RPM" />
    </OutputElements>
    <!-- 4. Внутренние состояния для мгновенной установки из игры -->
    <StateElements>
        <Element Name="FuelVolume" Type="Real" Component="FuelTank" InternalName="LiquidVolume" />
    </StateElements>
    <!-- 5. Внешние каналы управления -->
    <InputElements>
        <Element Name="Accelerator" Type="Real" />
        <Element Name="Ignition" Type="Integer" />
    </InputElements>
    <!-- 6. Маршрутизация каналов ввода в компоненты -->
    <InputProcessors>
        <Processor Input="Accelerator" Component="AbsTcs" />
        <Processor Input="Accelerator" Component="Turbocharger" InternalName="Throttle" />
        <Processor Input="Ignition" Component="IgnitionKey" />
    </InputProcessors>
</Vehicle>
```

### Пошаговый рецепт: перевод схемы на настоящий дизель `EngineDiesel` с турбиной `Turbocharger`

Все 6 стандартных схем в `Common/Vehicles/MotorCars/Schemes/` используют бензиновый класс `EngineGazoline` без турбокомпрессора. Чтобы сделать для мода полноценный турбодизель:

1. **Скопируйте базовую схему** (например, `AwdAuto.xml` и/или `AwdManual.xml`) из `Content/Common/Vehicles/MotorCars/Schemes/` в папку вашего мода `Plugins/Mods/Content/<ИмяМода>/`.
2. **В `<Components>`** замените `<Component Name="Engine" Class="EngineDiesel" />` и добавьте `<Component Name="Turbocharger" Class="Turbocharger" />` (перед `Idler` и `Engine`).
3. **В `<Links>` обязательно удалите связь `<Link Holder="IgnitionElectricCircuit" Reference="Engine" />`** (дизелю не нужна цепь искрового зажигания; если оставить эту связь, схема не загрузится!).
4. **В `<Links>` сохраните связь `<Link Holder="IgnitionKey" Reference="Idler" />`** (именно она перекрывает подачу топлива при повороте ключа в `0` и глушит дизель) и добавьте **`<Link Holder="Turbocharger" Reference="Engine" />`**.
5. **В `<InputProcessors>`** добавьте **`<Processor Input="Accelerator" Component="Turbocharger" InternalName="Throttle" />`**.
6. **Пропишите локальные схемы в `options.xml` и `Vehicle.ini`** (`../Plugins/Mods/Content/<ИмяМода>/AwdAuto.xml`) и добавьте секцию `[Turbocharger]` в `Vehicle.ini` (`MinRpm`, `RefRpm`, `RefCompressionRatio`, `LagTime = 0.08..0.10`).

---

## 5. Форматы таблиц кривых `.csv` и устройство `Engine.csv`

Во всех файлах `.csv` разделителем ячеек служит **точка с запятой `;`**, а разделителем дробной части числа — **строго точка `.`** (запятая вроде `8,8` приведёт к ошибке чтения!). Узлы аргументов должны идти строго по возрастанию без дубликатов.

### 5.1. Стандартные общие таблицы в `Common/Vehicles/MotorCars/Tables/`

| Файл таблицы в `Common/Vehicles/MotorCars/Tables/` | Формат | Где подключается в `Vehicle.ini` и что задаёт |
| --- | :-: | --- |
| **`SpringProgression.csv`** | 1D (2 строки) | `[Suspension*]` $\rightarrow$ `ProgressionCurve`: относительное сжатие пружины `[0..1]` $\rightarrow$ множитель жёсткости `[1.0..2.5]`. |
| **`EngineVibration.csv`** | 1D (2 строки) | `[Engine]` $\rightarrow$ `VibrationCurve`: нормированные обороты `RPM / IdleRpm` $\rightarrow$ амплитуда вибрации ДВС. |
| **`WheelForceLat.csv`** | 1D (2 строки) | `[Wheel*]` $\rightarrow$ `ForceCurveLateral`: угол бокового увода шины (град., `[0..180]`) $\rightarrow$ поперечная сила на нагрузке `ForceCurveLoad`. |
| **`WheelForceLon.csv`** | 1D (2 строки) | `[Wheel*]` $\rightarrow$ `ForceCurveLongitudinal`: продольное скольжение шины (`%`, `[0..200]`) $\rightarrow$ продольная сила на нагрузке `ForceCurveLoad`. |
| **`WheelAlignTorque.csv`** | 1D (2 строки) | `[Wheel*]` $\rightarrow$ `AligningTorqueCurve`: угол увода (град.) $\rightarrow$ стабилизирующий момент шины. *Внимание: в точке максимума `WheelForceLat.csv` (`10°`) значение в `WheelAlignTorque.csv` обязано быть строго равно `0`!* |
| **`WheelStaticAligningTorque.csv`** | 1D (2 строки) | `[Wheel*]` $\rightarrow$ `StaticAligningTorqueCurve`: нормированный угол `[-1..1]` $\rightarrow$ статический возвращающий момент на месте. |
| **`WheelRollingResistance.csv`** | 2D (матрица) | `[Wheel*]` $\rightarrow$ `RollingResistanceCurves`: строка 1 — относительное давление в шине `P / WorkingPressure` (`[0..1.5]`); столбец 1 — рыхлость покрытия (`[0..1]`); ячейки — множитель сопротивления качению. |
| **`WheelSagToCamberCurve.csv`** | 1D (2 строки) | `[Suspension*]` $\rightarrow$ `SagToCamberCurve`: обжатие подвески `[0..1]` $\rightarrow$ базовый угол развала колеса (град.). |
| **`WheelSteerToCamberCurve.csv`** | 1D (2 строки) | `[Suspension*]` $\rightarrow$ `SteerToCamberCurve`: угол поворота колеса `[-30..30]` (град.) $\rightarrow$ добавочный развал (град.). |
| **`WheelCamberToSlipAngle.csv`** | 1D (2 строки) | `[Wheel*]` $\rightarrow$ `CamberToSlipAngleCurve`: угол развала `[-5..5]` (град.) $\rightarrow$ смещение угла увода (град.). |

### 5.2. Формат одномерных таблиц (1D `.csv`)

Состоит ровно из **2 строк** с одинаковым числом столбцов (не менее 2 точек):

- **Строка 1:** значения аргумента $X_0; X_1; \dots; X_{n-1}$ по возрастанию.
- **Строка 2:** значения функции $Y_0; Y_1; \dots; Y_{n-1}$.

```csv
0;0.55;0.7;0.85;0.95;1
1;1;1.06;1.3;1.7;2.5
```

### 5.3. Точный формат таблицы двигателя `Engine.csv` (`PowerCurves`)

Файл `Engine.csv`, подключаемый через параметр `PowerCurves` в секции `[Engine]`, объединяет в себе 2D-карту индикаторного момента сгорания и 1D-кривую механических потерь (торможения двигателем):

1. **Строка 1 (шапка оборотов `RPM`):** первая ячейка — текст `thr/rpm` (или `0`), далее по возрастанию идут значения оборотов `RPM` (например, `0; 50; 250; 500; ...; 5500`).
2. **Строки 2..M (карта момента по положению педали газа):**
   - Первая ячейка каждой строки — **процент нажатия педали газа от `0` до `100`** (`0; 5; 10; 20; 40; 60; 80; 100`). При загрузке эти числа автоматически делятся на `100.0`, превращаясь в долю открытия дросселя `[0.0 .. 1.0]`.
   - Остальные ячейки строки — индикаторный крутящий момент сгорания в **$\text{Н}\cdot\text{м}$** при данных оборотах.
3. **Последняя строка (сразу после строки `100`):**
   - Первая ячейка — `0` (или `Friction`), остальные ячейки — **положительный момент внутренних механических потерь (торможения двигателем) $F(\text{RPM}) > 0$ в $\text{Н}\cdot\text{м}$** для каждого столбца оборотов. В последнем столбце за границей отсечки (`5500` или `8000 RPM`) обычно ставится заградительное значение `10000`.

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
> **Три важнейших правила расчёта `Engine.csv`:**
>
> 1. **Вычитание строки трения из всех строк:** На каждом кадре физическая модель двигателя вычитает последнюю строку механических потерь $F(\text{RPM})$ из момента сгорания **даже при 100% нажатии газа**:
>    $$T_{\text{маховик}}(\text{Throttle}, \text{RPM}) = T_{\text{таблица}}(\text{Throttle}, \text{RPM}) - F(\text{RPM}).$$
>    Поэтому, чтобы получить на маховике паспортный крутящий момент $T_{\text{DIN}}(\text{RPM})$ (например, $440\text{ Н}\cdot\text{м}$), для атмосферного мотора в строку `100` нужно записывать **сумму** $T_{\text{100}}(\text{RPM}) = T_{\text{DIN}}(\text{RPM}) + F(\text{RPM})$.
> 2. **Пересчёт таблицы при использовании `Turbocharger`:** Если к двигателю подключён компонент `Turbocharger`, он умножает момент сгорания из таблицы (до вычитания трения!) на коэффициент наддува:
>    $$\text{ratio}(\text{RPM}, \text{thr}) = 1 + (\text{RefCompressionRatio} - 1)\sqrt{\frac{\max(0,\; \text{RPM}\cdot\text{thr} - \text{MinRpm})}{\text{RefRpm} - \text{MinRpm}}}.$$
>    Поэтому при наличии `Turbocharger` каждая ячейка строк `0..100` (при $\text{thr} = \text{процент}/100$) вычисляется как $\big(T_{\text{целевой}}(\text{RPM}, \text{thr}) + F(\text{RPM})\big) / \text{ratio}(\text{RPM}, \text{thr})$, а последняя строка трения $F(\text{RPM})$ оставляется без деления!
> 3. **Удержание холостого хода под нагрузкой («анти-столл горб» — особенность симуляции `[Idler]`):** В текущей реализации регулятор холостого хода `FuelControllerComputer` (`[Idler]`) работает в разомкнутом цикле — он подбирает открытие заслонки только для компенсации собственного трения ДВС $F(\text{IdleRpm})$ и не увеличивает газ при просадке оборотов ниже `IdleRpm` от внешней нагрузки (гидротрансформатора в `D`, сцепления или генератора). Поэтому в таблице `Engine.csv` в строке `5%` на колонке `IdleRpm` (`750 RPM`) задают момент чуть выше трения $F(750)$, а на соседней нижней колонке (`500 RPM`) делают крутой подъём момента («анти-столл горб» в строках `5%–20%`), чтобы при просадке оборотов под нагрузкой крутящий момент резко возрастал и не давал мотору заглохнуть.

---

## 6. Сцепка тягача с прицепом (`<VehicleAttachment>`)

Для динамического объединения тягача (`Master`) и прицепа (`Slave`) используется отдельный XML-файл схемы сцепки с корневым тегом **`<VehicleAttachment>`**:

```xml
<?xml version="1.0" encoding="utf-8"?>
<VehicleAttachment>
    <!-- MasterLinks: Holder ищется в схеме тягача, Reference — в схеме прицепа -->
    <MasterLinks>
        <Link Holder="BatteryElectricCircuit" Reference="TrailerElectricCircuit" Connected="true" />
        <Link Holder="TractorMultiCircuitValve" Reference="TrailerAirReceiver" />
    </MasterLinks>

    <!-- SlaveLinks: Holder ищется в схеме прицепа, Reference — в схеме тягача -->
    <SlaveLinks>
    </SlaveLinks>
</VehicleAttachment>
```

При сцепке все одноимённые каналы управления (`Brake`, `ParkingBrake`, `LeftTurnLight`, `RightTurnLight`, `HazardLight`, `ParkingLights`, `LowBeam`) автоматически синхронно передаются от тягача во все прицепленные секции автопоезда.
