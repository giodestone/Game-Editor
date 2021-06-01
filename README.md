# Game Editor Extensions
![Camera Preview Moving](https://raw.githubusercontent.com/giodestone/Game-Editor/main/Images/GIF1.gif)

A game editor, [WFFC edit](https://github.com/savantguarde/WFFC-Edit), which is heavily based on Blizzards WOW editor, was extended to include a Unity style properties window (involving a bit of UI/UX design) as well as general improvements to code and code quality. This project was performed for a university coursework. Coded using C++ with the Microsoft Foundation Class (MFC) Library while interfacing with an SQLite (SQL) database.

## Running
Unfortunately, due to the way the provided project was set up, generating a standalone executable is not supported. To get this project running you must use Visual Studio with VC141 (2017) to run (2019 won't work). The project can be easily downloaded as zip [here](https://github.com/giodestone/Game-Editor/archive/refs/heads/main.zip)

## Application Architecture
![A scene object's properties are being updated.](https://raw.githubusercontent.com/giodestone/Game-Editor/main/Images/GIF2.gif)
The application is backed by an SQLite database where all of the information regarding the chunks and objects in the game is stored (each object has over 40 columns). The terrain data is represented by the `ChunkObject` class, and the game object data by the `SceneObject` class. Their rendering classes are represented by the `DisplayChunk` and `DisplayObject`, respectively.

The `MFCFrame.h` file (`CMyFrame` class) houses the main window and its logic. The `MFCRenderFrame.h` file (`CChildRender` class) contains the renderer logic. The `MFCMain` has the callbacks for logic from the `CMyFrame` window. The `ObjectPropertiesDialog` class houses the code for the properties window (main user facing focus of the app).

MFC, by design, uses a simple object orientated inheritance hierarchy. The `ObjectPropertiesDialog` inherit from `CDialog` and `CFloatEdit` inhertis from `CEdit`

## Changes to Editor
<img src="https://raw.githubusercontent.com/giodestone/Game-Editor/main/Images/Image4.jpg" height="500">

The source application can be found [here](https://github.com/savantguarde/WFFC-Edit).

* Addition of a Unity style object properties window.
    * Identifying features and frequently used features such as name and transform are exemplified by being placed at top.
    * Support for changing size and scaling.
    * `CFloatEdit` which is specifically for editing floats.
    * `CMFCPropertyGridCtrl` contains all properties, which are grouped into similar categories which have descriptions and titles.
        * Appropriate special edit controls are used, such as ones for changing colour (which include a preview).
        * Validation of texture/model paths.
        * `COleVariant` (equivalent of `std::variant`) is used to support native input validation for edit controls.
* Supporting hiding objects and displaying them as wire frame.
* Moving camera functionality to a dedicated `Camera` class and expanding the cameras functionality.
    * Adding mouse look (with consideration for frame/delta time).
    * Adding more comprehensive keyboard controls, such as going up/down and speeding up movement.
* Addition of a custom event system using interfaces (`IEventDispatcher` and `IEventReceiver`) to notify `ObjectPropertiesDialog` when `CFloatEdit` is updated.
* Changes to collections which store the scene graph and object list to use O(1) data structure (`std::unordered_map` aka a dictionary) and be indexed via the object's database ID (versus load order).
* Placeholder rendering if the path cannot be found.
* Moving selection logic to own class.
* Resolving type mismatch warnings by adding appropriate casts.
* Only updating the changed object in the scene graph to reduce computational costs (and improve UX).
* Save query now uses `UPDATE` instead of dropping all `SceneObject`s and inserting them again.
* Save icon is a save icon, not a smiley face.

## Read the Report
<img src="https://raw.githubusercontent.com/giodestone/Game-Editor/main/Images/Image3.jpg" height="500">

The report covers the specifics of the implementation, potential improvements, and has a UML diagram (among other things). It can be found [here](TODO).