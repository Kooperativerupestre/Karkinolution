from __future__ import annotations

class LimitedValue:
    def __init__(self, value:int | float, limit:int | float):
        self.value = value
        self.limit = limit
    def add(self, value:int | float) -> None:
        self.value = min(self.limit, self.value + value)
    def sub(self, value:int | float) -> None:
        self.value = max(0, self.value - value)
    def set(self, value:int | float) -> None:
        self.value = min(max(0, value), self.limit)

class Energy(LimitedValue):
    pass
class Life(LimitedValue):
    pass