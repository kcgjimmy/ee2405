/*
 * You can write copyrights rules here. These rules will be copied into the outputs.
 */

/*
 * Generated by erpcgen 1.9.0 on Sat May 28 22:30:22 2022.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#if !defined(_final_erpc_service_server_h_)
#define _final_erpc_service_server_h_

#ifdef __cplusplus
#include "erpc_server.h"
#include "erpc_codec.h"
extern "C"
{
#include "final_erpc_service.h"
#include <stdint.h>
#include <stdbool.h>
}

#if 10900 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif


/*!
 * @brief Service subclass for Final_ERPC.
 */
class Final_ERPC_service : public erpc::Service
{
public:
    Final_ERPC_service() : Service(kFinal_ERPC_service_id) {}

    /*! @brief Call the correct server shim based on method unique ID. */
    virtual erpc_status_t handleInvocation(uint32_t methodId, uint32_t sequence, erpc::Codec * codec, erpc::MessageBufferFactory *messageFactory);

private:
    /*! @brief Server shim for Get_data_from_mbed of Final_ERPC interface. */
    erpc_status_t Get_data_from_mbed_shim(erpc::Codec * codec, erpc::MessageBufferFactory *messageFactory, uint32_t sequence);
};

extern "C" {
#else
#include "final_erpc_service.h"
#endif // __cplusplus

typedef void * erpc_service_t;

erpc_service_t create_Final_ERPC_service(void);

#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
void destroy_Final_ERPC_service(erpc_service_t *service);
#elif ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_STATIC
void destroy_Final_ERPC_service(void);
#else
#warning "Unknown eRPC allocation policy!"
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _final_erpc_service_server_h_