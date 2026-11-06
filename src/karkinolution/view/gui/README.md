# GUI architecture

## Rendering dependencies

Raylib may be used by GUI implementation files, but public headers must not
include Raylib headers or expose Raylib types. Keep the rendering dependency
behind implementation boundaries to avoid leaking Raylib into includers.

Text measurement is provided by the font driver. It uses the same default-font
size clamp and character spacing rules as `DrawText`, so measured bounds match
the rendered text.

## Layout and geometric shapes

Layout is rectangular. Every component receives a rectangular slot from its
parent, while the component remains responsible for rendering and exact input
testing inside that slot. A circle uses the largest centered inscribed circle:
its diameter is the smaller slot extent.

This separation follows the layout model used by large UI systems such as
Unreal Slate and Godot Control: containers arrange rectangular widget geometry,
and widgets interpret that geometry for their own content and shape. It avoids
special cases in the layout engine and keeps future shapes local to their
components. The spatial index stores the conservative rectangular AABB;
collision queries still apply the component's exact geometry afterward.

References:

- [Unreal Slate layout](https://dev.epicgames.com/documentation/unreal-engine/understanding-the-slate-ui-architecture-in-unreal-engine?lang=en-US)
- [Godot Control layout](https://docs.godotengine.org/en/stable/classes/class_control.html)
