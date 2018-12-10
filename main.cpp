/* Copyright (c) 2018 Fraunhofer IIS and HelloSHORE contributors.
 *
 * This file is part of HelloSHORE which is released under MIT license.
 * See file LICENSE for full license details.
 */

#include <Shore.h>
#include <CreateFaceEngine.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>

#include <iostream>
#include <string>
#include <exception>

#define OUT_IMG "out.jpeg"

using namespace boost::gil;

// Draws a rectangle specified by its upper-left and buttom-right corners
void draw_rect( const rgb8_view_t &view,
                point2<unsigned int> upper_left,
                point2<unsigned int> bottom_right )
{
    unsigned int cols = 0;
    unsigned int rows = 0;
    auto loc_top      = view.x_at( upper_left.x, upper_left.y );
    auto loc_buttom   = view.x_at( upper_left.x, bottom_right.y );
    auto loc_left     = view.y_at( upper_left.x, upper_left.y );
    auto loc_right    = view.y_at( bottom_right.x, upper_left.y );
    rgb8_pixel_t color( 255, 255, 255 );

    while ( cols++ < bottom_right.x - upper_left.x ) {
        *loc_top++    = color;
        *loc_buttom++ = color;
    }
    while ( rows++ < bottom_right.y - upper_left.y ) {
        *loc_left++  = color;
        *loc_right++ = color;
    }
}

int main( int argc, char const *argv[] )
{
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::string;

    if ( argc < 2 ) {
        cout << "Usage: " << string( argv[0] ) << " testimage.jpeg" << endl;
        exit( -1 );
    }
    string img_path( argv[1] );
    // Read an image from file
    rgb8_image_t img;
    try {
        jpeg_read_image( img_path, img );
    } catch ( std::exception &ex ) {
        cerr << "Cannot read image file: " << img_path << endl;
        cerr << ex.what() << endl;
        exit( -1 );
    }
    rgb8_view_t img_view = view( img );

    // SHORE works with grayscaled input images - we have to convert
    gray8_image_t gray( img.dimensions() );
    gray8_view_t gray_view = view( gray );
    auto cc_view           = color_converted_view<gray8_pixel_t>( img_view );
    copy_pixels( cc_view, gray_view );

    const gray8_pixel_t &gray_px = *gray_view.pixels();
    unsigned char const *ptr_gray =
        reinterpret_cast<unsigned char const *>( &gray_px );

    // We do output all the SHORE messages to the standard output
    Shore::SetMessageCall(
        []( const char *msg ) -> void { cout << std::string( msg ) << endl; } );

    cout << "SHORE Version: " << string( Shore::Version() ) << endl;

    Shore::Engine *engine = Shore::CreateFaceEngine( 0,     // timeBase
                                                     false, // updateTimeBase
                                                     1,     // threadCount
                                                     "Face.Front", // modelType
                                                     1l,           // imageScale
                                                     96, // minFaceSize
                                                     9,  // minFaceScore
                                                     0,  // idMemoryLength
                                                     "Spatial", // idMemoryType
                                                     false,     // trackFaces
                                                     "Off",     // phantomTrap
                                                     true,      // searchEyes
                                                     false,     // searchNose
                                                     false,     // searchMouth
                                                     false,     // analyzeEyes
                                                     false,     // analyzeMouth
                                                     true,      // analyzeGender
                                                     true,      // analyzeAge
                                                     true,      // analyzeHappy
                                                     true,      // analyzeSad
                                                     true, // analyzeSurprised
                                                     true  // analyzeAngry
#if SHORE_VERSION >= 170
                                                     ,
                                                     "Off" // pointLocator
#endif
                                                     );

    if ( !engine ) {
        cerr << "Cannot create Shore::Engine" << endl;
        exit( -1 );
    }

    Shore::Content const *content =
        engine->Process( ptr_gray,
                         gray.width(),
                         gray.height(),
                         1,
                         sizeof( gray8_pixel_t ),
                         sizeof( gray8_pixel_t ) * gray.width(),
                         0,
                         "GRAYSCALE" );
    if ( !content ) {
        cerr << "Shore::Engine::Process failed" << endl;
        exit( -1 );
    }
    for ( unsigned int i = 0; i < content->GetObjectCount(); i++ ) {
        Shore::Object const *obj = content->GetObject( i );
        Shore::Region const *reg = content->GetObject( i )->GetRegion();

        draw_rect( img_view,
                   point2<unsigned int>( reg->GetLeft(), reg->GetTop() ),
                   point2<unsigned int>( reg->GetRight(), reg->GetBottom() ) );

        float const *val;
        char const *str_val;
        str_val          = obj->GetAttributeOf( "Gender" );
        string gender    = str_val ? string( str_val ) : "unknown";
        val              = obj->GetRatingOf( "Age" );
        string age       = val ? std::to_string( *val ) : "unknown";
        val              = obj->GetRatingOf( "Happy" );
        string happy     = val ? std::to_string( *val ) : "unknown";
        val              = obj->GetRatingOf( "Sad" );
        string sad       = val ? std::to_string( *val ) : "unknown";
        val              = obj->GetRatingOf( "Angry" );
        string angry     = val ? std::to_string( *val ) : "unknown";
        val              = obj->GetRatingOf( "Surprised" );
        string surprised = val ? std::to_string( *val ) : "unknown";

        cout << "Face #" << i << " : " << endl
             << "Gender: " << gender << endl
             << "Age: " << age << endl
             << "Happy: " << happy << endl
             << "Sad: " << sad << endl
             << "Angry: " << angry << endl
             << "Surprised: " << surprised << endl
             << endl;
    }

    Shore::DeleteEngine( engine );

    try {
        jpeg_write_view( OUT_IMG, img_view );
    } catch ( std::exception &ex ) {
        cerr << "Cannot write the image file: " << OUT_IMG << endl;
        cerr << ex.what() << endl;
        exit( -1 );
    }
    cout << "Results are written to " << OUT_IMG << endl;
    return 0;
}
