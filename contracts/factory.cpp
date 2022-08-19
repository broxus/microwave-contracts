// clang-format off
#include "../compiler/builtin.hpp"
// clang-format on

#include <tvm/contract.hpp>
#include <tvm/replay_attack_protection/timestamp.hpp>
#include <tvm/smart_switcher.hpp>

using namespace tvm;

__interface [[no_pubkey]] IFactory
{
    [[external, no_persistent]] virtual void constructor(address dest, cell state_init) = 0;
};

class Factory final : public smart_interface<IFactory> {
public:
    __always_inline void constructor(address dest, cell state_init)
    {
        auto message =
            builder().stu(0b010000, 6).stslice(dest.sl()).stvaruint16(0).stu(0b110, 1 + 4 + 4 + 64 + 32 + 2 + 1).stref(state_init).endc();
        tvm_sendmsg(message, 128 + 32);
    }
};

DEFINE_JSON_ABI(IFactory, empty, empty);

DEFAULT_MAIN_ENTRY_FUNCTIONS(Factory, IFactory, empty, 1800)
