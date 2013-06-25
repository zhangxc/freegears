#!/usr/bin/perl -w

use CGI qw/:standard/;

{
    my $cgi= new CGI;
    print $cgi->header,
    $cgi->start_html('A Simple Example'),
    $cgi->h1('A Simple Example');
    if ($cgi->param()) {
	print "Your name is ",
	$cgi->param('name');
    }

    $cgi->end_html();
}
