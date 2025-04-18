---
title: SpinhalfDistributed
---

A block in a spin $S=1/2$ Hilbert space with distributed computing capabilities.

**Sources**<br>
[spinhalf_distributed.hpp](https://github.com/awietek/xdiag/blob/main/xdiag/blocks/spinhalf_distributed.hpp)<br>
[spinhalf_distributed.cpp](https://github.com/awietek/xdiag/blob/main/xdiag/blocks/spinhalf_distributed.cpp)

---

## Constructors

=== "C++"	
	```c++
	SpinhalfDistributed(int64_t nsites, int64_t nup, std::string backend = "auto");
	```
	
| Name    | Description                                                                          | Default |
|:--------|:-------------------------------------------------------------------------------------|---------|
| nsites  | number of sites (integer)                                                            |         |
| nup     | number of "up" spin setting spin (integer)                                           |         |
| backend | backend used for coding the basis states                                             | `auto`  |
	
	
The parameter `backend` chooses how the block is coded internally. By using the default parameter `auto` the backend is chosen automatically. Alternatives are `32bit`, `64bit`.

---

## Iteration

An SpinhalfDistributed block can be iterated over, where at each iteration a [ProductState](../states/product_state.md) representing the corresponding basis state is returned.

=== "C++"	
	```c++
    auto block = SpinhalfDistributed(4, 2);
	for (auto pstate : block) {
	  Log("{} {}", to_string(pstate), block.index(pstate));
	}
	```
	
---

## Methods

#### index

Returns the index of a given [ProductState](../states/product_state.md) in the basis of the SpinhalfDistributed block.

=== "C++"	
	```c++
	int64_t index(SpinhalfDistributed const &block, ProductState const &pstate);
	```

---

#### nsites

Returns the number of sites of the block.

=== "C++"	
	```c++
	int64_t nsites(SpinhalfDistributed const &block);
	```
	
---

#### size
Returns the size of the block on a local process.

=== "C++"	
	```c++
	int64_t size(SpinhalfDistributed const &block) const;
	```


---

#### dim
Returns the dimension of the block, i.e. the sum of all sizes across all processes. 

=== "C++"	
	```c++
	int64_t dim(SpinhalfDistributed const &block) const;
	```
	

---
		
#### isreal
Returns whether the block can be used with real arithmetic. 
Complex arithmetic is needed when a
[Representation](../symmetries/representation.md) is genuinely complex.

=== "C++"	
	```c++
    bool isreal(SpinhalfDistributed const &block);
	```
