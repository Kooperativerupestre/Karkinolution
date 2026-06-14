from typing import Generic, TypeVar, Iterable, Tuple

K = TypeVar('K')
V = TypeVar('V')

class BaseStorage(Generic[K, V]):
    def __init__(self):
        self._data:dict[K, V] = {}
    @property
    def keys(self) -> Iterable[K]:
        return self._data.keys()
    @property
    def values(self) -> Iterable[V]:
        return self._data.values()
    @property
    def iter(self) -> Iterable[Tuple[K, V]]:
        return self._data.items()
    @property
    def invert(self) -> dict[V, K]:
        return {v: k for k, v in self.iter}
    
    def exists(self, key:K) -> bool:
        return key in self._data
        # O(1)
    def exists_value(self, value:V) -> bool:
        return value in self.values
        # O(n)

    def _already_exists_error(self, key:K) -> None:
        raise NotImplementedError
    def _not_found_error(self, key:K) -> None:
        raise NotImplementedError
    
    def add(self, key:K, value:V) -> None:
        if self.exists(key):
            self._already_exists_error(key)
        self._data[key] = value
    def delete(self, key:K) -> None:
        if not self.exists(key):
            self._not_found_error(key)
        del self._data[key]
    def get(self, key:K) -> V:
        if not self.exists(key):
            self._not_found_error(key)
        return self._data[key]
        # O(1)
    def get_key_by_value(self, value:V) -> K:
        inverse = self.invert
        if not value in inverse:
            raise ValueError('Value {} was not found'.format(value))
        return inverse[value]
        # O(n)
    def require(self, key:K) -> V | None:
        if not self.exists(key):
            return None
        return self._data[key]