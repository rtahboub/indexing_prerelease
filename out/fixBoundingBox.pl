#!/usr/bin/perl

#
# Perl script to fix the bounding box of a pdf file based on an eps file
#
# Usage: perl fixBoundingBox.pl <basedir> <plotName>
#  e.g.: perl fixBoundingBox.pl plotfiles/assocTrails trailScaleUpQuerying
#
# 12/13/10 sowell: Updated command line parameters to specify full path to
#                  eps/pdf file. This makes it easier to convert pdf files in
#                  arbitrarily nested directories.

sub printUsage {
  print "Usage: perl fixBoundingBox.pl <basedir> <plotName>\n";
}

# $basedir = $ARGV[0] || die(printUsage());
# $plotName = $ARGV[1] || die(printUsage());

$eps = $ARGV[0] || die(printUsage());
$pdf = $ARGV[1] || die(printUsage());



#$eps = "$basedir/eps/$plotName.eps";
#$pdf = "$basedir/pdf/$plotName.pdf";

print "eps file: $eps\n";
print "pdf file: $pdf\n";

open(EPS, $eps) || die("Could not open eps file");
@epsContent = <EPS>;
close(EPS);

open(PDF, "<$pdf") || die("Could not open pdf file");
binmode(PDF);
@pdfContent = <PDF>;
close(PDF);

# match bounding box in eps file
$epsString = join("\n", @epsContent);
if ($epsString =~ m/%%BoundingBox:\s*(\d+\s+\d+\s+\d+\s+\d+)/) {
  $boundingBox = $1;
  print "Bounding box: [$boundingBox]\n";

  # replace bounding box in pdf file
  open(PDF, ">$pdf") || die("Could not open pdf file for writing");
  binmode(PDF);
  foreach $pdfString (@pdfContent) {
    $pdfString =~ s/MediaBox(\s*)\[\d+\s+\d+\s+\d+\s+\d+\]/MediaBox${1}[${boundingBox}]/g;
    print PDF $pdfString;
  }
  close(PDF);
}


