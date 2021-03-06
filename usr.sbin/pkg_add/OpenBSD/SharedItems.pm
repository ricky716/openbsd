# ex:ts=8 sw=4:
# $OpenBSD: src/usr.sbin/pkg_add/OpenBSD/SharedItems.pm,v 1.31 2014/02/06 16:55:01 espie Exp $
#
# Copyright (c) 2004-2006 Marc Espie <espie@openbsd.org>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

use strict;
use warnings;

package OpenBSD::SharedItems;

use OpenBSD::Error;
use OpenBSD::PackageInfo;
use OpenBSD::PackingList;
use OpenBSD::Paths;

sub find_items_in_installed_packages
{
	my $state = shift;
	my $db = OpenBSD::SharedItemsRecorder->new;
	$state->status->what("Read")->object("shared items");
	$state->progress->for_list("Read shared items", [installed_packages()],
	    sub {
		my $e = shift;
		my $plist = OpenBSD::PackingList->from_installation($e,
		    \&OpenBSD::PackingList::SharedItemsOnly) or return;
		return if !defined $plist;
		$plist->record_shared($db, $e);
	    });
	return $db;
}

sub check_shared
{
	my ($set, $o) = @_;
	if (!defined $set->{db}) {
		$set->{db} = OpenBSD::SharedItemsRecorder->new;
		for my $pkg (installed_packages()) {
			next if $set->{older}{$pkg};
			my $plist = OpenBSD::PackingList->from_installation($pkg,
			    \&OpenBSD::PackingList::SharedItemsOnly) or next;
			next if !defined $plist;
			$plist->record_shared($set->{db}, $pkg);
		}
	}
	if (defined $set->{db}{dirs}{$o->fullname}) {
		return 1;
	} else {
		push(@{$set->{old_shared}{$o->fullname}}, $o);
		return 0;
	}
}

sub wipe_directory
{
	my ($state, $h, $d) = @_;

	my $realname = $state->{destdir}.$d;

	for my $i (@{$h->{$d}}) {
		$state->log->set_context('-'.$i->{pkgname});
		$i->cleanup($state);
	}
	if (!rmdir $realname) {
		$state->log("Error deleting directory #1: #2",
		    $realname, $!)
			unless $state->{dirs_okay}{$d};
		return 0;
	}
	return 1;
}

sub cleanup
{
	my ($recorder, $state) = @_;

	my $remaining = find_items_in_installed_packages($state);

	$state->progress->clear;
	$state->status->what("Clean");
	$state->progress->set_header("Clean shared items");
	my $h = $recorder->{dirs};
	my $u = $recorder->{users};
	my $g = $recorder->{groups};
	my $total = 0;
	$total += keys %$h if defined $h;
	$total += keys %$u if defined $u;
	$total += keys %$g if defined $g;
	my $done = 0;

	for my $d (sort {$b cmp $a} keys %$h) {
		$state->progress->show($done, $total);
		my $realname = $state->{destdir}.$d;
		if (defined $remaining->{dirs}{$realname}) {
			for my $i (@{$h->{$d}}) {
				$state->log->set_context('-'.$i->{pkgname});
				$i->reload($state);
			}
		} else {
			wipe_directory($state, $h, $d);
		}
		$done++;
	}
	while (my ($user, $pkgname) = each %$u) {
		$state->progress->show($done, $total);
		next if $remaining->{users}{$user};
		if ($state->{extra}) {
			$state->system(OpenBSD::Paths->userdel, '--',
			    $user);
		} else {
			$state->log->set_context('-'.$pkgname);
			$state->log("You should also run /usr/sbin/userdel #1", $user);
		}
		$done++;
	}
	while (my ($group, $pkgname) = each %$g) {
		$state->progress->show($done, $total);
		next if $remaining->{groups}{$group};
		if ($state->{extra}) {
			$state->system(OpenBSD::Paths->groupdel, '--',
			    $group);
		} else {
			$state->log->set_context('-'.$pkgname);
			$state->log("You should also run /usr/sbin/groupdel #1", $group);
		}
		$done++;
	}
	if ($state->verbose >= 2) {
		$state->progress->next;
	} else {
		$state->progress->clear;
	}
}

package OpenBSD::PackingElement;
sub cleanup
{
}

sub reload
{
}

package OpenBSD::PackingElement::Mandir;
sub cleanup
{
	my ($self, $state) = @_;
	my $fullname = $state->{destdir}.$self->fullname;
	$state->log("You may wish to remove #1 from man.conf", $fullname);
	for my $f (OpenBSD::Paths->man_cruft) {
		unlink("$fullname/$f");
	}
}

package OpenBSD::PackingElement::Fontdir;
sub cleanup
{
	my ($self, $state) = @_;
	my $fullname = $state->{destdir}.$self->fullname;
	$state->log("You may wish to remove #1 from your font path", $fullname);
	for my $f (OpenBSD::Paths->font_cruft) {
		unlink("$fullname/$f");
	}
}

package OpenBSD::PackingElement::Infodir;
sub cleanup
{
	my ($self, $state) = @_;
	my $fullname = $state->{destdir}.$self->fullname;
	for my $f (OpenBSD::Paths->info_cruft) {
		unlink("$fullname/$f");
	}
}

1;
