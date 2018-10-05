//
//  PPComputedShapeSource.m
//  iosapp
//
//  Created by Albin Törnqvist on 2018-10-05.
//  Copyright © 2018 Mapbox. All rights reserved.
//

#import "PPComputedShapeSourceDataSource.h"

@implementation PPComputedShapeSourceDataSource

- (NSArray<MGLShape<MGLFeature> *> *)featuresInTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)zoomLevel {
    if ((x == 4399 || x == 4398) && (y == 2465 || y == 2466)) {

        usleep(1e5);

        NSString *geoJSON = @"{\"type\": \"Feature\", \"geometry\": {\"type\": \"MultiPolygon\", \"coordinates\": [[[[13.31727907, 58.032889709999999],[13.317506140000001, 58.03295284],[13.31754888, 58.032920609999998],[13.317288169999999, 58.032760660000001],[13.31713407, 58.032623360000002],[13.316997799999999, 58.032556669999998],[13.31699118, 58.032548830000003],[13.316434490000001, 58.032135689999997],[13.316159580000001, 58.031956710000003],[13.316041589999999, 58.031854959999997],[13.31593374, 58.031738310000001],[13.315752509999999, 58.031575599999996],[13.31561729, 58.031438549999997],[13.31538817, 58.031290239999997],[13.315167539999999, 58.031162080000001],[13.31502023, 58.031047630000003],[13.314968500000001, 58.031058530000003],[13.31492356, 58.031083930000001],[13.31469598, 58.031212549999999],[13.316629649999999, 58.032699010000002],[13.316812990000001, 58.032686890000001],[13.317016499999999, 58.032739239999998],[13.31727907, 58.032889709999999]]]]},\"properties\": {}}";
        NSData *data = [geoJSON dataUsingEncoding:NSUTF8StringEncoding];
        MGLShape<MGLFeature> *shape = (MGLShape<MGLFeature> *)[MGLPolygonFeature shapeWithData:data encoding:NSUTF8StringEncoding error:NULL];
        NSLog(@"Loaded!");
        return @[shape];
    } else {
        return nil;
    }
}

@end
