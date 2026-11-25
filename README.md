<div align="center">

# CCD2_VehicleKit

**Official Vehicle Modding Kit for City Car Driving 2.0 (Unreal Engine 5.8)**

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.8-313131?logo=unrealengine&logoColor=white)](https://www.unrealengine.com/)
[![FMOD Studio](https://img.shields.io/badge/FMOD%20Studio-2.03.14-000000)](https://github.com/Forward-Development/CCD2_VehicleKit_FMOD)
[![Docs EN](https://img.shields.io/badge/Docs-English-0969da)](./Docs/EN/ModdingGuide_MD/README.md)
[![Docs RU](https://img.shields.io/badge/Docs-%D0%A0%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B9-1a7f37)](./Docs/RU/ModdingGuide_MD/README.md)

[English](#english) • [Русский](#русский)

</div>

---

<a id="english"></a>

## English

**`CCD2_VehicleKit`** is the template Unreal Engine 5.8 project and documentation suite for creating, tuning, packaging, and publishing custom vehicles for **City Car Driving 2.0**.

It includes a fully configured sample vehicle (`CieBensVGruppe`), pre-built parent materials (`MI_Mod_*`), physics configuration samples, an automated `.pak` build script (`build_mod_dlc.bat`), and complete modular guides written for **both beginners (with zero Unreal Engine experience) and experienced modders**.

### Quick Start (5 Steps)

1. **Install Unreal Engine 5.8** from the Epic Games Launcher *(no Visual Studio or extra SDKs required)*.
2. **Download [`CCD2_VehicleKit.zip`](https://github.com/Forward-Development/CCD2_VehicleKit/releases/latest)** from the **Releases** page *(or clone with [Git LFS](https://git-lfs.com/): `git lfs install && git clone https://github.com/Forward-Development/CCD2_VehicleKit.git`)* and extract into a short path using Latin characters only and no spaces (e.g., `D:\Modding\CCD2_VehicleKit\`).
   > **Do NOT use `Code → Download ZIP` or `Source code (zip)`!** GitHub's auto-generated ZIP archives do not include Git LFS binary files (`.dll`, `.uasset`, `.umap`), which causes `UnrealEditor.exe - Bad Image (0xc000012f)` on launch.
3. **Open `CityCarDriving2.uproject`** and enable **`Settings → Show Plugin Content`** in the Content Browser to access `Plugins/Mods Content/`.
4. **Follow [Module 1 — Minimal Car Quick Start](./Docs/EN/ModdingGuide_MD/01_Minimal_Car_QuickStart.md)** to assemble a drivable vehicle (body + 4 wheels).
5. **Run [`build_mod_dlc.bat`](./build_mod_dlc.bat)**, rename `\Saved\DLC\Windows\Mods.pak` to `<YourModName>.pak`, and test locally in `<GameDir>\CityCarDriving2\Content\Paks\`.

---

### Documentation (`Docs/EN/`)

> [!TIP]
> Browse the **Markdown** versions right here on GitHub, or open the standalone **HTML** versions locally in your browser (`Docs/EN/ModdingGuide_HTML/` and `Docs/EN/VehicleLib_HTML/`).

#### Vehicle Modding Guide (Step-by-Step)
| Module | Topic | Description |
| :-: | --- | --- |
| **—** | **[Overview & UE 5.8 Basics](./Docs/EN/ModdingGuide_MD/README.md)** | Installation, UE editor mini-guide for beginners, glossary, mod folder structure, and `CieBensVGruppe` sample. |
| **01** | **[Minimal Car (Quick Start)](./Docs/EN/ModdingGuide_MD/01_Minimal_Car_QuickStart.md)** | **Bare essentials (MVP):** Blender/FBX export, body + 4 wheels, basic materials, minimal physics, specification asset, `.pak` build, local test & Steam Workshop upload. |
| **02** | **[Exterior, Lights & World](./Docs/EN/ModdingGuide_MD/02_Exterior_Lights_and_World.md)** | Full exterior PBR materials, brake calipers (`Suspension*`), emissive lamps & real spotlights, license plates, `LogicBox`, `FuelTank`, and `RainBlocker`. |
| **03** | **[Interior, Dashboard & Cameras](./Docs/EN/ModdingGuide_MD/03_Interior_Dashboard_Cameras_Seats.md)** | Cabin interior (`InteriorObjects`), steering wheel, animated needles & warning icons (`SalonLights`), all 5 cameras, driver seating (`Seats`), and cabin toys (`CarToy`). |
| **04** | **[Mirrors & Wipers](./Docs/EN/ModdingGuide_MD/04_Mirrors_and_Wipers.md)** | Live rear-view mirrors (`Camera` socket + UV script + `Mirror`) and skeletal-animated windshield wipers (`Wippers`). |
| **05** | **[Sounds & FMOD Studio](./Docs/EN/ModdingGuide_MD/05_Sounds_and_FMOD.md)** | Positioning/selecting built-in game sounds (`SoundEvent`) and authoring custom sound banks in FMOD Studio 2.03.14. |
| **06** | **[Specs, Logic & Cheat Sheet](./Docs/EN/ModdingGuide_MD/06_Advanced_Specs_Logic_and_Reference.md)** | Vehicle card stats (`CustomSpecifications`), switchable options (`MultiSpecifications`), custom Blueprint `ActorComponent` logic, and the master reference table. |

#### Vehicle Physics Reference (`VehicleLib`)
| Section | Topic |
| :-: | --- |
| **—** | **[README — Architecture, All 46 Component Classes & Units](./Docs/EN/VehicleLib_MD/README.md)** |
| **00** | **[Physics Quick Start — Formulas & 5 Verification Tests](./Docs/EN/VehicleLib_MD/00_Quick_Start.md)** |
| **01** | **[Architecture, `Vehicle.ini`, `options.xml`, XML Schemas & `.csv`](./Docs/EN/VehicleLib_MD/01_Architecture_and_Config.md)** |
| **02** | **[Named Inputs, Outputs & States (`*Intensity`, Channels)](./Docs/EN/VehicleLib_MD/02_Named_IO_and_States.md)** |
| **03** | **[TorqueTrain Components (Engines, Clutch, Gearboxes, Brakes, Tires)](./Docs/EN/VehicleLib_MD/03_Components_TorqueTrain.md)** |
| **04** | **[Electrical System (`Battery`, `ElectricCircuit`, `CarElectrics`)](./Docs/EN/VehicleLib_MD/04_Components_Electrics.md)** |
| **05** | **[Chassis & Subsystems (Suspension, Steering, Turbo, Aero, ABS/TCS)](./Docs/EN/VehicleLib_MD/05_Components_Chassis_and_Subsystems.md)** |

---

### Repository Layout

```text
CCD2_VehicleKit/
├── CityCarDriving2.uproject     # Open in Unreal Engine 5.8
├── build_mod_dlc.bat            # One-click .pak builder -> Saved/DLC/Windows/Mods.pak
├── Plugins/Mods/Content/
│   ├── CieBensVGruppe/          # Complete reference vehicle (excluded from builds via L_IGNORE)
│   └── <YourModName>/           # Create your vehicle mod folder here
└── Docs/
    ├── EN/                      # English docs (ModdingGuide_MD/HTML + VehicleLib_MD/HTML)
    ├── RU/                      # Russian docs (ModdingGuide_MD/HTML + VehicleLib_MD/HTML)
    └── Samples/VehicleLibConfigs/  # Ready-to-copy physics configs (Vehicle.ini, options.xml, Engine.csv)
```

### Useful Links
- **Custom Audio FMOD Templates:** [Forward-Development/CCD2_VehicleKit_FMOD](https://github.com/Forward-Development/CCD2_VehicleKit_FMOD) *(requires FMOD Studio 2.03.14)*
- **Steam Workshop Publishing Guide:** [Steam Community Guide (Step 4)](https://steamcommunity.com/sharedfiles/filedetails/?id=3763865332)

---

<a id="русский"></a>

## Русский

**`CCD2_VehicleKit`** — официальный шаблонный проект Unreal Engine 5.8 и комплект документации для создания, настройки физики, упаковки и публикации пользовательских автомобилей для **City Car Driving 2.0**.

Внутри уже есть полностью настроенный автомобиль-пример (`CieBensVGruppe`), родительские материалы (`MI_Mod_*`), примеры конфигов физики, скрипт сборки `.pak` (`build_mod_dlc.bat`) и пошаговая модульная документация, понятная **даже тем, кто никогда раньше не работал с Unreal Engine**.

### Быстрый старт (5 шагов)

1. **Установите Unreal Engine 5.8** через Epic Games Launcher *(Visual Studio и дополнительные SDK не требуются)*.
2. **Скачайте [`CCD2_VehicleKit.zip`](https://github.com/Forward-Development/CCD2_VehicleKit/releases/latest)** со страницы **Releases** *(или склонируйте с установленным [Git LFS](https://git-lfs.com/): `git lfs install && git clone https://github.com/Forward-Development/CCD2_VehicleKit.git`)* и распакуйте в папку с коротким путём на латинице и без пробелов (например, `D:\Modding\CCD2_VehicleKit\`).
   > **Не используйте `Code → Download ZIP` и `Source code (zip)`!** Автоматические ZIP-архивы GitHub не включают бинарные файлы Git LFS (`.dll`, `.uasset`, `.umap`), из-за чего при запуске возникает ошибка `UnrealEditor.exe - Bad Image (0xc000012f)`.
3. **Откройте `CityCarDriving2.uproject`** и включите **`Settings → Show Plugin Content`** в Content Browser, чтобы увидеть `Plugins/Mods Content/`.
4. **Пройдите [Модуль 1 — Минимальный автомобиль](./Docs/RU/ModdingGuide_MD/01_Minimal_Car_QuickStart.md)**, чтобы собрать и запустить в игре минимально рабочую машину (кузов + 4 колеса).
5. **Запустите [`build_mod_dlc.bat`](./build_mod_dlc.bat)**, переименуйте `\Saved\DLC\Windows\Mods.pak` в `<ИмяМода>.pak` и проверьте локально в `<ПапкаИгры>\CityCarDriving2\Content\Paks\`.

---

### Документация (`Docs/RU/`)

> [!TIP]
> Читайте **Markdown**-версию прямо на GitHub или открывайте локальные **HTML**-версии в браузере (`Docs/RU/ModdingGuide_HTML/` и `Docs/RU/VehicleLib_HTML/`).

#### Руководство по созданию автомобилей (по модулям)
| Модуль | Раздел | Содержание |
| :-: | --- | --- |
| **—** | **[Обзор и основы UE 5.8](./Docs/RU/ModdingGuide_MD/README.md)** | Установка, мини-гид по интерфейсу UE для новичков, словарь терминов, структура папки мода и пример `CieBensVGruppe`. |
| **01** | **[Минимальный автомобиль (Быстрый старт)](./Docs/RU/ModdingGuide_MD/01_Minimal_Car_QuickStart.md)** | **Самое необходимое (MVP):** экспорт из Blender/FBX, кузов + 4 колеса, базовые материалы, минимальная физика, спецификация, сборка `.pak`, локальный тест и публикация в Steam Workshop. |
| **02** | **[Экстерьер, оптика и мир](./Docs/RU/ModdingGuide_MD/02_Exterior_Lights_and_World.md)** | Полный набор PBR-материалов экстерьера, суппорты (`Suspension*`), светящиеся слоты и реальные источники света, номера, `LogicBox`, `FuelTank` и `RainBlocker`. |
| **03** | **[Салон, приборка и камеры](./Docs/RU/ModdingGuide_MD/03_Interior_Dashboard_Cameras_Seats.md)** | Интерьер (`InteriorObjects`), руль, стрелки и индикаторы приборной панели (`SalonLights`), все 5 камер, посадка водителя (`Seats`) и место для игрушки (`CarToy`). |
| **04** | **[Зеркала и дворники](./Docs/RU/ModdingGuide_MD/04_Mirrors_and_Wipers.md)** | Зеркала заднего вида (сокет `Camera` + UV-скрипт + `Mirror`) и анимированные дворники (`Wippers`). |
| **05** | **[Звуки и FMOD Studio](./Docs/RU/ModdingGuide_MD/05_Sounds_and_FMOD.md)** | Расстановка и выбор встроенных звуков игры (`SoundEvent`), а также создание собственных банков в FMOD Studio 2.03.14. |
| **06** | **[Комплектации, логика и шпаргалка](./Docs/RU/ModdingGuide_MD/06_Advanced_Specs_Logic_and_Reference.md)** | Характеристики карточки (`CustomSpecifications`), переключаемые опции (`MultiSpecifications`), пользовательская логика в `ActorComponent` и единая таблица-шпаргалка. |

#### Справочник по настройке физики (`VehicleLib`)
| Раздел | Содержание |
| :-: | --- |
| **—** | **[README — Архитектура, 46 классов компонентов и единицы измерения](./Docs/RU/VehicleLib_MD/README.md)** |
| **00** | **[Быстрый старт физики — Формулы расчёта и 5 контрольных тестов](./Docs/RU/VehicleLib_MD/00_Quick_Start.md)** |
| **01** | **[Архитектура, `Vehicle.ini`, `options.xml`, XML-схемы и `.csv`](./Docs/RU/VehicleLib_MD/01_Architecture_and_Config.md)** |
| **02** | **[Именованный ввод/вывод и состояния (`*Intensity`, каналы)](./Docs/RU/VehicleLib_MD/02_Named_IO_and_States.md)** |
| **03** | **[Компоненты трансмиссии (ДВС, сцепление, КПП, тормоза, шины)](./Docs/RU/VehicleLib_MD/03_Components_TorqueTrain.md)** |
| **04** | **[Бортовая электрика (`Battery`, `ElectricCircuit`, `CarElectrics`)](./Docs/RU/VehicleLib_MD/04_Components_Electrics.md)** |
| **05** | **[Шасси и подсистемы (подвеска, рулевое, турбо, аэродинамика, ABS/TCS)](./Docs/RU/VehicleLib_MD/05_Components_Chassis_and_Subsystems.md)** |

### Полезные ссылки
- **Шаблоны FMOD для своих звуков:** [Forward-Development/CCD2_VehicleKit_FMOD](https://github.com/Forward-Development/CCD2_VehicleKit_FMOD) *(требуется FMOD Studio 2.03.14)*
- **Инструкция по публикации в Steam Workshop:** [Руководство в Steam (пункт 4)](https://steamcommunity.com/sharedfiles/filedetails/?id=3763865332)
