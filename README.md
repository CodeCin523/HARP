# HARP

**HARP** is a minimal runtime that orchestrates modular behavior through runtime-loaded packages. Each package contributes handlers and actors that define the system’s capabilities and flow. HARP focuses on lifecycle, connection, and hot-swapping, keeping the core lightweight while maintaining a coherent whole as components evolve independently and in concert.

## Concept
- **HARP** : Handler-Actor Runtime Package
- **Handler** : a singleton performing a single responsibility, possibly depending on other handlers.
- **Actor** : linked to a single parent handler and inherits access to the handler’s dependencies.
- **Creator** : a factory-like entity responsible for creating handlers, actors, or other components, with optional flags to control behavior.
- **Base** : the foundational first element in a type, enabling uniform casting and pointer-based referencing across the system.
- **Descriptor** : a lightweight description of a package, handler, actor, or API, containing its identity, version, and callbacks for lifecycle management.
