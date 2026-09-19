# No Silent Degradation

If a change makes behavior less correct, less tested, or less safe than what was there before — SAY SO. Explicitly. In
the commit message and in the response to the user.

Forbidden:

- Removing a bounds check "to simplify" without flagging it
- Downgrading an assert to a comment
- Skipping a test case because it's "an edge case that probably doesn't matter" — in a spatial index, every edge case
  matters
- Silently catching an exception instead of propagating it

If you are unsure whether a change is a regression, assume it is and say so. Enzo will decide if the tradeoff is
acceptable. You don't get to decide that for him.