class SimulationError(Exception):
    pass

class IdError(SimulationError):
    pass
class CoordinateError(SimulationError):
    pass

class EntityError(SimulationError):
    pass

# REPRODUCTIVE SUBDOMAIN

class ReproductiveError(EntityError):
    pass

class GenderError(ReproductiveError):
    pass

class AlreadyPregnantError(ReproductiveError):
    pass

class DifferentSpeciesError(ReproductiveError):
    pass

# ID SUBDOMAIN
class IdNotFoundError(IdError):
    pass
class IdAlreadyExistsError(IdError):
    pass
# COORD SUBDOMAIN
class CoordNotFoundError(CoordinateError):
    pass
class CoordAlreadyExistError(CoordinateError):
    pass

# MOVEMENT SUBDOMAIN

class NonMotileError(SimulationError):
    pass
# ENTITY TYPE ERROR

class EntityTypeError(EntityError):
    pass
# ENERGY TYPE ERROR

class InsufficientEnergyError(EntityError):
    pass