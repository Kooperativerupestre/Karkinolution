# View Conventions & Architecture

## Namespace Prefixes

- `K-` -> `KarkinolutionView`
- `KG-` -> `Opsiamethystos::Gui`

## Architectural Boundaries

### Opsiaméthystos vs View

- `opsiamethystos` is a domain-agnostic, generic UI toolkit responsible strictly for rectangular layouts, quadtree spatial indexing, input hit-testing, and generic component rendering. It has no knowledge of simulation concepts, biology, or networking protocols.
- `app` is a presentation consumer that composes `opsiamethystos` UI components, Raylib window lifecycle, and client-side networking.

### Simulation Runner Migration

The simulation core lives in `karkinolution/`, while the executable application lives in the repository-root `app/` directory.
The application does not own or simulate a parallel world state; it only visualizes received client data and issues client requests.

## Threading & Synchronization

- **Main Thread Invariant**: The UI rendering loop executes on the main thread (targeting 60 FPS) and must **never block** on network I/O or pending request waits.
- **Background I/O**: Network transport runs on a background worker thread managed by `RuntimeClient` via `asio::io_context`.
- **Synchronizer Boundary**: `KarkinolutionView::Synchronizer` coordinates connection state and thread boundaries, ensuring that UI draws query local state without stalling the window loop.

