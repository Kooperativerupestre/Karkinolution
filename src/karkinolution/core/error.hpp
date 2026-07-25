#pragma once
#include <stdexcept>
#include <string>

// BASE SIMULATION EXCEPTION
class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& message) : std::runtime_error(message) {}
};

// MAIN SUBDOMAINS
class IdError : public SimulationError {
public:
    explicit IdError(const std::string& message) : SimulationError(message) {}
};

class CoordinateError : public SimulationError {
public:
    explicit CoordinateError(const std::string& message) : SimulationError(message) {}
};

class EntityError : public SimulationError {
public:
    explicit EntityError(const std::string& message) : SimulationError(message) {}
};

class NonMotileError : public SimulationError {
public:
    explicit NonMotileError(const std::string& message) : SimulationError(message) {}
};

// REPRODUCTIVE SUBDOMAIN (Inherits from EntityError)
class ReproductiveError : public EntityError {
public:
    explicit ReproductiveError(const std::string& message) : EntityError(message) {}
};

class GenderError : public ReproductiveError {
public:
    explicit GenderError(const std::string& message) : ReproductiveError(message) {}
};

class AlreadyPregnantError : public ReproductiveError {
public:
    explicit AlreadyPregnantError(const std::string& message) : ReproductiveError(message) {}
};

class DifferentSpeciesError : public ReproductiveError {
public:
    explicit DifferentSpeciesError(const std::string& message) : ReproductiveError(message) {}
};

// ID SUBDOMAIN (Inherits from IdError)
class IdNotFoundError : public IdError {
public:
    explicit IdNotFoundError(const std::string& message) : IdError(message) {}
};

class IdAlreadyExistsError : public IdError {
public:
    explicit IdAlreadyExistsError(const std::string& message) : IdError(message) {}
};

// COORD SUBDOMAIN (Inherits from CoordinateError)
class CoordinateNotFoundError : public CoordinateError {
public:
    explicit CoordinateNotFoundError(const std::string& message) : CoordinateError(message) {}
};

class CoordinateAlreadyExistsError : public CoordinateError {
public:
    explicit CoordinateAlreadyExistsError(const std::string& message) : CoordinateError(message) {}
};

// ENTITY TYPE ERROR (Inherits from EntityError)
class EntityTypeError : public EntityError {
public:
    explicit EntityTypeError(const std::string& message) : EntityError(message) {}
};

// ENERGY TYPE ERROR (Inherits from EntityError)
class InsufficientEnergyError : public EntityError {
public:
    explicit InsufficientEnergyError(const std::string& message) : EntityError(message) {}
};