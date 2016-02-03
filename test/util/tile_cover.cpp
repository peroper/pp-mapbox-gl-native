#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/source_info.hpp>

#include <gtest/gtest.h>

#include <unordered_set>

using namespace mbgl;
using set = std::unordered_set<TileID>;

TEST(TileCover, Empty) {
    auto result = tileCover(LatLngBounds::empty(), 0, 0);
    ASSERT_TRUE(result.empty());
}

TEST(TileCover, Arctic) {
    auto result = tileCover(LatLngBounds::hull({ 86, -180 }, { 90, 180 }), 0, 0);
    ASSERT_TRUE(result.empty());
}

TEST(TileCover, Antarctic) {
    auto result = tileCover(LatLngBounds::hull({ -86, -180 }, { -90, 180 }), 0, 0);
    ASSERT_TRUE(result.empty());
}

TEST(TileCover, WorldZ0) {
    auto result = tileCover(LatLngBounds::world(), 0, 0);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(0, result[0].z);
    ASSERT_EQ(0, result[0].x);
    ASSERT_EQ(0, result[0].y);
}

TEST(TileCover, WorldZ1) {
    auto result = tileCover(LatLngBounds::world(), 1, 1);
    ASSERT_EQ(4, result.size());
    ASSERT_EQ(
        (set {{
            TileID(1, 1, 1, 1),
            TileID(1, 0, 1, 1),
            TileID(1, 1, 0, 1),
            TileID(1, 0, 0, 1)
        }}),
        (set {
            result.begin(),
            result.end()
        }));
}

//TEST(TileCover, SingletonZ0) {
//    auto result = tileCover(LatLngBounds::singleton({0, 0}), 0, 0);
//    ASSERT_EQ(1, result.size());
//    ASSERT_EQ(0, result[0].z);
//    ASSERT_EQ(0, result[0].x);
//    ASSERT_EQ(0, result[0].y);
//}
//
//TEST(TileCover, SingletonZ1) {
//    auto result = tileCover(LatLngBounds::singleton({0, 0}), 1, 1);
//    ASSERT_EQ(1, result.size());
//    ASSERT_EQ(0, result[0].z);
//    ASSERT_EQ(0, result[0].x);
//    ASSERT_EQ(0, result[0].y);
//}

static const LatLngBounds sanFrancisco = LatLngBounds::hull(
    { 37.6609, -122.5744 },
    { 37.8271, -122.3204 });

TEST(TileCover, SanFranciscoZ0) {
    auto result = tileCover(sanFrancisco, 0, 0);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(0, result[0].w);
    ASSERT_EQ(0, result[0].z);
    ASSERT_EQ(0, result[0].x);
    ASSERT_EQ(0, result[0].y);
}

TEST(TileCover, SanFranciscoZ10) {
    auto result = tileCover(sanFrancisco, 10, 10);
    ASSERT_EQ(4, result.size());
    ASSERT_EQ(
        (set {{
            TileID(10, 163, 395, 10),
            TileID(10, 164, 395, 10),
            TileID(10, 163, 396, 10),
            TileID(10, 164, 396, 10)
        }}),
        (set {
            result.begin(),
            result.end()
        }));
}

//TEST(TileCover, OrderedByDistanceToCenter) {
//    auto result = tileCover(sanFrancisco, 12, 12);
//    ASSERT_EQ(12, result.size());
//    ASSERT_EQ(  12, result[0].z);
//    ASSERT_EQ( 654, result[0].x);
//    ASSERT_EQ(1583, result[0].y);
//    ASSERT_EQ(  12, result[1].z);
//    ASSERT_EQ( 655, result[1].x);
//    ASSERT_EQ(1583, result[1].y);
//}

static const LatLngBounds sanFranciscoWrapped = LatLngBounds::hull(
    { 37.6609, 238.5744 },
    { 37.8271, 238.3204 });

//TEST(TileCover, SanFranciscoZ0Wrapped) {
//    auto result = tileCover(sanFranciscoWrapped, 0, 0);
//    ASSERT_EQ(1, result.size());
//    ASSERT_EQ(1, result[0].w);
//    ASSERT_EQ(0, result[0].z);
//    ASSERT_EQ(0, result[0].x);
//    ASSERT_EQ(0, result[0].y);
//}

TEST(TileCover, OfflineEmptyBounds) {
    SourceInfo info;
    auto result = tileCover(LatLngBounds::empty(), 0, 20, SourceType::Vector, 512, info);
    ASSERT_TRUE(result.empty());
}

TEST(TileCover, OfflineEmptyZoomIntersection) {
    SourceInfo info;
    info.minZoom = 6;
    info.maxZoom = 20;
    auto result = tileCover(LatLngBounds::world(), 0, 5, SourceType::Vector, 512, info);
    ASSERT_TRUE(result.empty());
}

TEST(TileCover, OfflineZoomIntersection) {
    SourceInfo info;
    info.minZoom = 0;
    info.maxZoom = 2;
    auto result = tileCover(sanFrancisco, 1, 3, SourceType::Vector, 512, info);
    ASSERT_EQ(2, result.size());
}

TEST(TileCover, Offline512PxTileAtZ0) {
    SourceInfo info;
    auto result = tileCover(LatLngBounds::world(), 0, 0, SourceType::Vector, 512, info);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(0, result[0].z);
}

TEST(TileCover, Offline256PxTileAtZ0) {
    SourceInfo info;
    auto result = tileCover(LatLngBounds::world(), 0, 0, SourceType::Vector, 256, info);
    ASSERT_EQ(4, result.size());
    ASSERT_EQ(1, result[0].z);
}

TEST(TileCover, OfflineVectorTileRounding) {
    SourceInfo info;
    auto result = tileCover(sanFranciscoWrapped, 0.6, 0.7, SourceType::Vector, 512, info);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(0, result[0].z);
}

TEST(TileCover, OfflineRasterTileRounding) {
    SourceInfo info;
    auto result = tileCover(sanFranciscoWrapped, 0.6, 0.7, SourceType::Raster, 512, info);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(1, result[0].z);
}

TEST(TileCover, OfflineWrapped) {
    SourceInfo info;
    auto result = tileCover(sanFranciscoWrapped, 0, 0, SourceType::Vector, 512, info);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(0, result[0].z);
    ASSERT_EQ(0, result[0].x);
    ASSERT_EQ(0, result[0].y);
}
