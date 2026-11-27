# API Binary Adapter (APIBYA)

APIBYA connects the binary server boundary to a domain operation. World-resource adapters call the Karkinolution
domain API; system-call adapters use the connection's `SimulationAdapter` for server-scoped operations such as
listing worlds, acquiring a world, and creating one.

The adapter exists to encapsulate the transition from binary data to domain data and from the domain result back to
binary data in one place. It interprets request payloads, invokes the appropriate domain or simulation operation, and
constructs the binary response. This keeps protocol translation out of the domain API and prevents server request
handling from duplicating serialization and deserialization logic. The frame processor remains responsible for
dispatching calls to the operation-specific adapters.