/*
 * exempi - test2.cpp
 *
 * Copyright (C) 2007 Hubert Figuiere
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1 Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2 Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * 3 Neither the name of the Authors, nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software wit hout specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <string>

#include <boost/static_assert.hpp>
#include <boost/test/auto_unit_test.hpp>

#include "utils.h"
#include "xmp.h"
#include "xmpconsts.h"

using boost::unit_test::test_suite;


void test_xmpfiles_write()
{
	BOOST_CHECK(xmp_init());

	XmpFilePtr f = xmp_files_open_new(g_testfile.c_str(), XMP_OPEN_READ);

	BOOST_CHECK(f != NULL);
	if (f == NULL) {
		return;
	}

	XmpPtr xmp = xmp_files_get_new_xmp(f);
	BOOST_CHECK(xmp != NULL);

	BOOST_CHECK(xmp_files_free(f));

	char buf[1024];
	snprintf(buf, 1024, "cp %s test.jpg ; chmod u+w test.jpg", g_testfile.c_str());
	BOOST_CHECK(system(buf) != -1);
	
	f = xmp_files_open_new("test.jpg", XMP_OPEN_FORUPDATE);

	BOOST_CHECK(f != NULL);
	if (f == NULL) {
		return;
	}

	BOOST_CHECK(xmp_set_property(xmp, NS_PHOTOSHOP, "ICCProfile", "foo", 0));

	BOOST_CHECK(xmp_files_can_put_xmp(f, xmp));
	BOOST_CHECK(xmp_files_put_xmp(f, xmp));

	BOOST_CHECK(xmp_free(xmp));
	BOOST_CHECK(xmp_files_close(f, XMP_CLOSE_SAFEUPDATE));
	BOOST_CHECK(xmp_files_free(f));

	f = xmp_files_open_new("test.jpg", XMP_OPEN_READ);

	BOOST_CHECK(f != NULL);
	if (f == NULL) {
		return;
	}
	xmp = xmp_files_get_new_xmp(f);
	BOOST_CHECK(xmp != NULL);

	XmpStringPtr the_prop = xmp_string_new();
	BOOST_CHECK(xmp_get_property(xmp, NS_PHOTOSHOP, "ICCProfile", the_prop, NULL));
	BOOST_CHECK_EQUAL(strcmp("foo", xmp_string_cstr(the_prop)),	0); 

	xmp_string_free(the_prop);

	BOOST_CHECK(xmp_free(xmp));
	BOOST_CHECK(xmp_files_close(f, XMP_CLOSE_NOOPTION));
	BOOST_CHECK(xmp_files_free(f));

//	unlink("test.jpg");
	xmp_terminate();

	BOOST_CHECK(!g_lt->check_leaks());
	BOOST_CHECK(!g_lt->check_errors());
}


void test_xmpfiles()
{
	BOOST_CHECK(xmp_init());

	XmpFilePtr f = xmp_files_open_new(g_testfile.c_str(), XMP_OPEN_READ);

	BOOST_CHECK(f != NULL);
	if (f == NULL) {
		exit(128);
	}

	XmpPtr xmp = xmp_new_empty();

	BOOST_CHECK(xmp != NULL);
	
	BOOST_CHECK(xmp_files_get_xmp(f, xmp));

	XmpStringPtr the_prop = xmp_string_new();

	BOOST_CHECK(xmp_get_property(xmp, NS_PHOTOSHOP, "ICCProfile", the_prop, NULL));
	BOOST_CHECK_EQUAL(strcmp("sRGB IEC61966-2.1", xmp_string_cstr(the_prop)),	0); 

	xmp_string_free(the_prop);
	BOOST_CHECK(xmp_free(xmp));

	BOOST_CHECK(xmp_files_free(f));
	xmp_terminate();

	BOOST_CHECK(!g_lt->check_leaks());
	BOOST_CHECK(!g_lt->check_errors());
}


/**  See http://www.adobeforums.com/webx/.3bc42b73 for the orignal
 *   test case */
void 
test_tiff_leak()
{
	std::string orig_tiff_file = g_src_testdir 
		+ "../../samples/BlueSquares/BlueSquare.tif";
	std::string command = "cp ";
	command += orig_tiff_file + " test.tif";
	BOOST_CHECK(system(command.c_str()) >= 0);
	BOOST_CHECK(chmod("test.tif", S_IRUSR|S_IWUSR) == 0);
	BOOST_CHECK(xmp_init());

	XmpFilePtr f = xmp_files_open_new("test.tif", XMP_OPEN_FORUPDATE);

	BOOST_CHECK(f != NULL);
	if (f == NULL) {
		return;
	}

	XmpPtr xmp;
	
	BOOST_CHECK(xmp = xmp_files_get_new_xmp(f));
	BOOST_CHECK(xmp != NULL);

	xmp_set_localized_text(xmp, NS_DC, "description", "en", "en-US", "foo", 0);
	BOOST_CHECK(xmp_files_put_xmp(f, xmp));
	BOOST_CHECK(xmp_files_close(f, XMP_CLOSE_NOOPTION));
	BOOST_CHECK(xmp_free(xmp));
	BOOST_CHECK(xmp_files_free(f));
	xmp_terminate();

	BOOST_CHECK(unlink("test.tif") == 0);
	BOOST_CHECK(!g_lt->check_leaks());
	BOOST_CHECK(!g_lt->check_errors());	
}


test_suite*
init_unit_test_suite( int argc, char * argv[] ) 
{
    test_suite* test = BOOST_TEST_SUITE("test xmpfiles");
	
	prepare_test(argc, argv, "../../samples/BlueSquares/BlueSquare.jpg");
	
	test->add(BOOST_TEST_CASE(&test_xmpfiles));
	test->add(BOOST_TEST_CASE(&test_xmpfiles_write));
	test->add(BOOST_TEST_CASE(&test_tiff_leak));
	
    return test;
}

