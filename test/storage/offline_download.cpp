#include "../fixtures/stub_file_source.hpp"

#include <mbgl/storage/offline.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/offline_download.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/io.hpp>

#include <gtest/gtest.h>
#include <iostream>

using namespace mbgl;
using namespace std::literals::string_literals;

class MockObserver : public OfflineRegionObserver {
public:
    void statusChanged(OfflineRegionStatus status) override {
        if (statusChangedFn) statusChangedFn(status);
    };

    void error(std::exception_ptr err) override {
        if (errorFn) errorFn(err);
    };

    std::function<void (OfflineRegionStatus)> statusChangedFn;
    std::function<void (std::exception_ptr)> errorFn;
};

class OfflineTest {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    OfflineDatabase db { ":memory:", fileSource };
    std::size_t size = 0;

    Response response(const std::string& path) {
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/"s + path));
        size += response.data->length();
        return response;
    }
};

TEST(OfflineDownload, NoSubresources) {
    OfflineTest test;
    OfflineDownload download(
        1,
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/offline/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/style.json", resource.url);
        return test.response("offline/empty.style.json");
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(status.completedResourceCount, 1);
            EXPECT_EQ(status.completedResourceSize, test.size);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, InlineSource) {
    OfflineTest test;
    OfflineDownload download(
        1,
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/offline/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/style.json", resource.url);
        return test.response("offline/inline_source.style.json");
    };

    test.fileSource.tileResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/{z}-{x}-{y}.vector.pbf", resource.tileData->urlTemplate);
        EXPECT_EQ(1, resource.tileData->pixelRatio);
        EXPECT_EQ(0, resource.tileData->x);
        EXPECT_EQ(0, resource.tileData->y);
        EXPECT_EQ(0, resource.tileData->z);
        return test.response("offline/0-0-0.vector.pbf");
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(status.completedResourceCount, 2);
            EXPECT_EQ(status.completedResourceSize, test.size);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, GeoJSONSource) {
    OfflineTest test;
    OfflineDownload download(
        1,
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/offline/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/style.json", resource.url);
        return test.response("offline/geojson_source.style.json");
    };

    test.fileSource.sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/geojson.json", resource.url);
        return test.response("offline/geojson.json");
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(status.completedResourceCount, 2);
            EXPECT_EQ(status.completedResourceSize, test.size);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}

TEST(OfflineDownload, Activate) {
    OfflineTest test;
    OfflineDownload download(
        1,
        OfflineTilePyramidRegionDefinition("http://127.0.0.1:3000/offline/style.json", LatLngBounds::world(), 0.0, 0.0, 1.0),
        test.db, test.fileSource);

    test.fileSource.styleResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/style.json", resource.url);
        return test.response("offline/style.json");
    };

    test.fileSource.spriteImageResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/sprite.png", resource.url);
        return test.response("offline/sprite.png");
    };

    test.fileSource.spriteJSONResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/sprite.json", resource.url);
        return test.response("offline/sprite.json");
    };

    test.fileSource.glyphsResponse = [&] (const Resource&) {
        return test.response("offline/glyph.pbf");
    };

    test.fileSource.sourceResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/streets.json", resource.url);
        return test.response("offline/streets.json");
    };

    test.fileSource.tileResponse = [&] (const Resource& resource) {
        EXPECT_EQ("http://127.0.0.1:3000/offline/{z}-{x}-{y}.vector.pbf", resource.tileData->urlTemplate);
        EXPECT_EQ(1, resource.tileData->pixelRatio);
        EXPECT_EQ(0, resource.tileData->x);
        EXPECT_EQ(0, resource.tileData->y);
        EXPECT_EQ(0, resource.tileData->z);
        return test.response("offline/0-0-0.vector.pbf");
    };

    auto observer = std::make_unique<MockObserver>();

    observer->statusChangedFn = [&] (OfflineRegionStatus status) {
        if (status.complete()) {
            EXPECT_EQ(status.completedResourceCount, 261); // 256 glyphs, 1 tile, 1 style, source, sprite image, and sprite json
            EXPECT_EQ(status.completedResourceSize, test.size);
            test.loop.stop();
        }
    };

    download.setObserver(std::move(observer));
    download.setState(OfflineRegionDownloadState::Active);

    test.loop.run();
}
