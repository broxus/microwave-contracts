// clang-format off
#include "../compiler/builtin.hpp"
// clang-format on

#include <tvm/contract.hpp>
#include <tvm/incoming_msg.hpp>
#include <tvm/reflection.hpp>
#include <tvm/smart_switcher.hpp>

using namespace tvm;

#ifdef deploy
#undef deploy
#endif

static constexpr int ERROR_INVALID_ABI = 60;

__interface [[no_pubkey, no_timestamp, no_expire]] IMicrowave
{
    [[internal, noaccept, no_persistent]] virtual void deploy(MsgAddress dest, cell state_init) = 0;
};

constexpr auto make_json_abi()
{
    using MethodCount = get_interface_methods_count<IMicrowave>;
    constexpr auto functions = tvm::json_abi_gen::make_json_abi_impl<IMicrowave, 0, MethodCount::value>::value;
    return R"({"ABI version": 2,"version": "2.2","functions": [)"_s + functions + R"(],"events": [],"headers": []})"_s;
}

const char* json_abi = make_json_abi().c_str();

[[tvm_raw_func]] int main_internal(tvm::cell msg, tvm::slice msg_body)
{
    tvm::slice msg_slice = msg.ctos();
    require(!is_bounced(msg_slice), ERROR_INVALID_ABI);

    if (!msg_body.empty() || !msg_body.srempty()) {
        parser msg_parser(msg_body);
        unsigned func_id = msg_parser.ldu(32);
        tvm_assert(!!func_id, ERROR_INVALID_ABI);

        require(func_id == tvm::get_func_id<IMicrowave, 0>(), ERROR_INVALID_ABI);

        const auto dest = msg_parser.ldmsgaddr();
        const auto state_init = msg_parser.ldref();

        tvm::tvm_rawreserve(1'000'000'000, tvm::rawreserve_flag::none);

        auto message =
            builder().stu(0b010000, 6).stslice(dest).stvaruint16(0).stu(0b110, 1 + 4 + 4 + 64 + 32 + 2 + 1).stref(state_init).endc();
        tvm_sendmsg(message, 128);
    }

    return 0;
}
