#pragma once

#include <type_traits>

#define CPPREALM_HAVE_GENERATED_BRIDGE_TYPES

namespace realm::internal::bridge::storage {
    using OwnedBinaryData = std::aligned_storage<16, 8>::type;
    using Realm_Config = std::aligned_storage<432, 8>::type;
    using ColKey = std::aligned_storage<8, 8>::type;
    using Decimal128 = std::aligned_storage<16, 8>::type;
    using Dictionary = std::aligned_storage<80, 8>::type;
    using CoreDictionary = std::aligned_storage<248, 8>::type;
    using List = std::aligned_storage<80, 8>::type;
    using LnkLst = std::aligned_storage<240, 8>::type;
    using Mixed = std::aligned_storage<24, 8>::type;
    using ObjKey = std::aligned_storage<8, 8>::type;
    using ObjLink = std::aligned_storage<16, 8>::type;
    using Obj = std::aligned_storage<64, 8>::type;
    using ObjectId = std::aligned_storage<12, 1>::type;
    using ObjectSchema = std::aligned_storage<192, 8>::type;
    using Object = std::aligned_storage<104, 8>::type;
    using IndexSet = std::aligned_storage<32, 8>::type;
    using CollectionChangeSet = std::aligned_storage<272, 8>::type;
    using IndexSet_IndexIterator = std::aligned_storage<64, 8>::type;
    using IndexSet_IndexIteratableAdaptor = std::aligned_storage<8, 8>::type;
    using NotificationToken = std::aligned_storage<24, 8>::type;
    using Property = std::aligned_storage<184, 8>::type;
    using Query = std::aligned_storage<160, 8>::type;
    using Results = std::aligned_storage<824, 8>::type;
    using Schema = std::aligned_storage<32, 8>::type;
    using Set = std::aligned_storage<80, 8>::type;
    using TableRef = std::aligned_storage<16, 8>::type;
    using TableView = std::aligned_storage<440, 8>::type;
    using ThreadSafeReference = std::aligned_storage<8, 8>::type;
    using Timestamp = std::aligned_storage<16, 8>::type;
    using UUID = std::aligned_storage<16, 1>::type;
    using Status = std::aligned_storage<8, 8>::type;
    using ErrorCategory = std::aligned_storage<4, 4>::type;

}
