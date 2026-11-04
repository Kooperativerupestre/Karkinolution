# GUI architecture

## Rendering dependencies

Raylib may be used by GUI implementation files, but public headers must not
include Raylib headers or expose Raylib types. Keep the rendering dependency
behind implementation boundaries to avoid leaking Raylib into includers.
