# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libkingpepe_cli*         | RPC client functionality used by *kingpepe-cli* executable |
| *libkingpepe_common*      | Home for common functionality shared by different executables and libraries. Similar to *libkingpepe_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libkingpepe_consensus*   | Stable, backwards-compatible consensus functionality used by *libkingpepe_node* and *libkingpepe_wallet* and also exposed as a [shared library](../shared-libraries.md). |
| *libkingpepeconsensus*    | Shared library build of static *libkingpepe_consensus* library |
| *libkingpepe_kernel*      | Consensus engine and support library used for validation by *libkingpepe_node* and also exposed as a [shared library](../shared-libraries.md). |
| *libkingpepeqt*           | GUI functionality used by *kingpepe-qt* and *kingpepe-gui* executables |
| *libkingpepe_ipc*         | IPC functionality used by *kingpepe-node*, *kingpepe-wallet*, *kingpepe-gui* executables to communicate when [`--enable-multiprocess`](multiprocess.md) is used. |
| *libkingpepe_node*        | P2P and RPC server functionality used by *kingpeped* and *kingpepe-qt* executables. |
| *libkingpepe_util*        | Home for common functionality shared by different executables and libraries. Similar to *libkingpepe_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libkingpepe_wallet*      | Wallet functionality used by *kingpeped* and *kingpepe-wallet* executables. |
| *libkingpepe_wallet_tool* | Lower-level wallet functionality used by *kingpepe-wallet* executable. |
| *libkingpepe_zmq*         | [ZeroMQ](../zmq.md) functionality used by *kingpeped* and *kingpepe-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. Exceptions are *libkingpepe_consensus* and *libkingpepe_kernel* which have external interfaces documented at [../shared-libraries.md](../shared-libraries.md).

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`libkingpepe_*_SOURCES`](../../src/Makefile.am) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libkingpepe_node* code lives in `src/node/` in the `node::` namespace
  - *libkingpepe_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libkingpepe_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libkingpepe_util* code lives in `src/util/` in the `util::` namespace
  - *libkingpepe_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

kingpepe-cli[kingpepe-cli]-->libkingpepe_cli;

kingpeped[kingpeped]-->libkingpepe_node;
kingpeped[kingpeped]-->libkingpepe_wallet;

kingpepe-qt[kingpepe-qt]-->libkingpepe_node;
kingpepe-qt[kingpepe-qt]-->libkingpepeqt;
kingpepe-qt[kingpepe-qt]-->libkingpepe_wallet;

kingpepe-wallet[kingpepe-wallet]-->libkingpepe_wallet;
kingpepe-wallet[kingpepe-wallet]-->libkingpepe_wallet_tool;

libkingpepe_cli-->libkingpepe_util;
libkingpepe_cli-->libkingpepe_common;

libkingpepe_common-->libkingpepe_consensus;
libkingpepe_common-->libkingpepe_util;

libkingpepe_kernel-->libkingpepe_consensus;
libkingpepe_kernel-->libkingpepe_util;

libkingpepe_node-->libkingpepe_consensus;
libkingpepe_node-->libkingpepe_kernel;
libkingpepe_node-->libkingpepe_common;
libkingpepe_node-->libkingpepe_util;

libkingpepeqt-->libkingpepe_common;
libkingpepeqt-->libkingpepe_util;

libkingpepe_wallet-->libkingpepe_common;
libkingpepe_wallet-->libkingpepe_util;

libkingpepe_wallet_tool-->libkingpepe_wallet;
libkingpepe_wallet_tool-->libkingpepe_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class kingpepe-qt,kingpeped,kingpepe-cli,kingpepe-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Consensus* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libkingpepe_wallet* and *libkingpepe_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libkingpepe_consensus* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libkingpepe_util* should also be a standalone dependency that any library can depend on, and it should not depend on other internal libraries.

- *libkingpepe_common* should serve a similar function as *libkingpepe_util* and be a place for miscellaneous code used by various daemon, GUI, and CLI applications and libraries to live. It should not depend on anything other than *libkingpepe_util* and *libkingpepe_consensus*. The boundary between _util_ and _common_ is a little fuzzy but historically _util_ has been used for more generic, lower-level things like parsing hex, and _common_ has been used for kingpepe-specific, higher-level things like parsing base58. The difference between util and common is mostly important because *libkingpepe_kernel* is not supposed to depend on *libkingpepe_common*, only *libkingpepe_util*. In general, if it is ever unclear whether it is better to add code to *util* or *common*, it is probably better to add it to *common* unless it is very generically useful or useful particularly to include in the kernel.


- *libkingpepe_kernel* should only depend on *libkingpepe_util* and *libkingpepe_consensus*.

- The only thing that should depend on *libkingpepe_kernel* internally should be *libkingpepe_node*. GUI and wallet libraries *libkingpepeqt* and *libkingpepe_wallet* in particular should not depend on *libkingpepe_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libkingpepe_consensus*, *libkingpepe_common*, and *libkingpepe_util*, instead of *libkingpepe_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libkingpepeqt*, *libkingpepe_node*, *libkingpepe_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](`../../src/interfaces/`) abstract interfaces.

## Work in progress

- Validation code is moving from *libkingpepe_node* to *libkingpepe_kernel* as part of [The libkingpepekernel Project #24303](https://github.com/kingpepe/kingpepe/issues/24303)
- Source code organization is discussed in general in [Library source code organization #15732](https://github.com/kingpepe/kingpepe/issues/15732)
