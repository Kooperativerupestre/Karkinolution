# API Binary Adapter (APIBYA)

APIBYA connects the binary server boundary to the Karkinolution domain API. It accepts binary request data, interprets
that data as domain inputs, calls the appropriate API operation, and converts the result back into a binary response.

The adapter exists to encapsulate the transition from binary data to domain data and from the domain result back to
binary data in one place. This keeps protocol translation out of the domain API and prevents server request handling
from duplicating serialization and deserialization logic.