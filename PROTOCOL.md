# Frame Protocol

## Common frame prefix

Every frame begins with the same six-byte prefix. The prefix is shared by all
frame scopes so a transport can determine whether a complete frame has
arrived before dispatching it to a scope-specific parser.

| Field | Size | Meaning |
|---|---:|---|
| `declared_size` | 4 bytes | Number of bytes after this field, including `type`, `sub_type`, and the rest of the frame |
| `type` | 1 byte | Scope-specific frame kind |
| `sub_type` | 1 byte | Scope-specific operation or output kind |

Numeric values use the binary conventions defined by the shared binary layer.
The declared size is therefore also the length needed to calculate the total
frame size:

```text
total_frame_size = sizeof(declared_size) + declared_size
```

The smallest valid declared size is two bytes, because `type` and `sub_type`
are always included in the declared portion.

## Scope-specific interpretation

World and System use the same byte positions but have independent enum types.
The numeric values are not globally unique; a value such as `1` can have a
different meaning in each scope. The base parser therefore receives the
expected scope at compile time and returns variants that preserve that scope.

The public World and System parsers unwrap their corresponding variant after
the base parser has read the common prefix. Scope-specific validators remain
responsible for rejecting unknown type and subtype values.

## Frame availability

The shared frame detector uses only the common prefix and `declared_size`:

- `NO`: no bytes are available;
- `DECLARED_SIZE_INCOMPLETE`: the four-byte size field has not arrived;
- `DECLARED_SIZE_COMPLETE`: only the size field has arrived;
- `INCOMPLETE`: the common prefix exists, but the declared total is not present;
- `YES`: the complete declared frame is present.

After common detection succeeds, each scope may apply its own larger fixed
header requirement. This preserves the common transport invariant without
pretending that World and System have identical payload layouts.

## Compatibility rule

The common prefix is the stable protocol boundary. New scope-specific fields
must be appended after the prefix and must be accounted for by that scope's
declared-size and payload calculations. The base layer must not encode
domain-specific payload rules.
