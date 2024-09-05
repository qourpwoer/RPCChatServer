#pragma once

#include "OfflineService.pb.h"

class OfflineService : public ik_OfflineService::OfflineServiceRpc
{
    void WriteOffline(google::protobuf::RpcController *controller,
                              const ::ik_OfflineService::WriteOfflineRequest *request,
                              google::protobuf::Empty *response,
                              ::google::protobuf::Closure *done);
    void ReadOffline(google::protobuf::RpcController *controller,
                             const ::ik_OfflineService::ReadOfflineRequest *request,
                             ::ik_OfflineService::ReadOfflineResponse *response,
                             ::google::protobuf::Closure *done);
};