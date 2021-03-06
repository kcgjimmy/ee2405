/*
 * You can write copyrights rules here. These rules will be copied into the outputs.
 */

/*
 * Generated by erpcgen 1.9.0 on Sat May 28 22:30:22 2022.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#include "final_erpc_service_server.h"
#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
#include <new>
#include "erpc_port.h"
#endif
#include "erpc_manually_constructed.h"

#if 10900 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

using namespace erpc;
using namespace std;

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
#endif

ERPC_MANUALLY_CONSTRUCTED_STATIC(Final_ERPC_service, s_Final_ERPC_service);



// Call the correct server shim based on method unique ID.
erpc_status_t Final_ERPC_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    erpc_status_t erpcStatus;
    switch (methodId)
    {
        case kFinal_ERPC_Get_data_from_mbed_id:
        {
            erpcStatus = Get_data_from_mbed_shim(codec, messageFactory, sequence);
            break;
        }

        default:
        {
            erpcStatus = kErpcStatus_InvalidArgument;
            break;
        }
    }

    return erpcStatus;
}

// Server shim for Get_data_from_mbed of Final_ERPC interface.
erpc_status_t Final_ERPC_service::Get_data_from_mbed_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint8_t data;
    float result;

    // startReadMessage() was already called before this shim was invoked.

    codec->read(&data);

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        result = Get_data_from_mbed(data);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kFinal_ERPC_service_id, kFinal_ERPC_Get_data_from_mbed_id, sequence);

        codec->write(result);

        err = codec->getStatus();
    }

    return err;
}

#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
erpc_service_t create_Final_ERPC_service()
{
    return new (nothrow) Final_ERPC_service();
}

void destroy_Final_ERPC_service(erpc_service_t *service)
{
    if (*service)
    {
        delete service;
    }
}
#elif ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_STATIC
erpc_service_t create_Final_ERPC_service()
{
    s_Final_ERPC_service.construct();
    return s_Final_ERPC_service.get();
}

void destroy_Final_ERPC_service()
{
    s_Final_ERPC_service.destroy();
}
#else
#warning "Unknown eRPC allocation policy!"
#endif
