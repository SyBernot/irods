from nose.plugins.skip import SkipTest
from pydevtest_common import assertiCmd, assertiCmdFail, interruptiCmd
import pydevtest_sessions as s
import commands
import os
import shlex
import datetime
import time

class ShortAndSuite(object):

    def run_resource_setup(self):
        print "run_resource_setup - BEGIN"
        for i in self.my_test_resource["setup"]:
            parameters = shlex.split(i) # preserves quoted substrings
            print s.adminsession.runAdminCmd(parameters[0],parameters[1:])
        print "run_resource_setup - END"

    def run_resource_teardown(self):
        print "run_resource_teardown - BEGIN"
        for i in self.my_test_resource["teardown"]:
            parameters = shlex.split(i) # preserves quoted substrings
            print s.adminsession.runAdminCmd(parameters[0],parameters[1:])
        print "run_resource_teardown - END"

    def test_awesome(self):
        print "AWESOME!"

class ResourceSuite(object):
    '''Define the tests to be run for a resource type.

    This class is designed to be used as a base class by developers
    when they write tests for their own resource plugins.

    All these tests will be inherited and the developer can add
    any new tests for new functionality or replace any tests
    they need to modify.
    '''

    def run_resource_setup(self):
        for i in self.my_test_resource["setup"]:
            parameters = shlex.split(i) # preserves quoted substrings
            print s.adminsession.runAdminCmd(parameters[0],parameters[1:])

    def run_resource_teardown(self):
        for i in self.my_test_resource["teardown"]:
            parameters = shlex.split(i) # preserves quoted substrings
            print s.adminsession.runAdminCmd(parameters[0],parameters[1:])

    # SKIP TEST
    def test_skip_me(self):
        raise SkipTest

    ###################
    # iget
    ###################

    def test_local_iget(self):
        # local setup
        localfile = "local.txt"
        # assertions
        assertiCmd(s.adminsession,"iget "+s.testfile+" "+localfile) # iget
        output = commands.getstatusoutput( 'ls '+localfile )
        print "  output: ["+output[1]+"]"
        assert output[1] == localfile
        # local cleanup
        output = commands.getstatusoutput( 'rm '+localfile )

    def test_local_iget_with_overwrite(self):
        # local setup
        localfile = "local.txt"
        # assertions
        assertiCmd(s.adminsession,"iget "+s.testfile+" "+localfile) # iget
        assertiCmdFail(s.adminsession,"iget "+s.testfile+" "+localfile) # already exists
        assertiCmd(s.adminsession,"iget -f "+s.testfile+" "+localfile) # already exists, so force
        output = commands.getstatusoutput( 'ls '+localfile )
        print "  output: ["+output[1]+"]"
        assert output[1] == localfile
        # local cleanup
        output = commands.getstatusoutput( 'rm '+localfile )

    def test_local_iget_with_bad_option(self):
        # assertions
        assertiCmdFail(s.adminsession,"iget -z") # run iget with bad option

    ###################
    # imv
    ###################

    def test_local_imv(self):
        # local setup
        movedfile = "moved_file.txt"
        # assertions
        assertiCmd(s.adminsession,"imv "+s.testfile+" "+movedfile) # move
        assertiCmd(s.adminsession,"ils -L "+movedfile,"LIST",movedfile) # should be listed
        # local cleanup

    def test_local_imv_to_directory(self):
        # local setup
        # assertions
        assertiCmd(s.adminsession,"imv "+s.testfile+" "+s.testdir) # move
        assertiCmd(s.adminsession,"ils -L "+s.testdir,"LIST",s.testfile) # should be listed
        # local cleanup

    def test_local_imv_to_existing_filename(self):
        # local setup
        copyfile = "anotherfile.txt"
        # assertions
        assertiCmd(s.adminsession,"icp "+s.testfile+" "+copyfile) # icp
        assertiCmd(s.adminsession,"imv "+s.testfile+" "+copyfile, "ERROR", "CAT_NAME_EXISTS_AS_DATAOBJ") # cannot overwrite existing file
        # local cleanup

    ###################
    # iput
    ###################

    def test_local_iput(self):
        # local setup
        datafilename = "newfile.txt"
        f = open(datafilename,'wb')
        f.write("TESTFILE -- ["+datafilename+"]")
        f.close()
        # assertions
        assertiCmdFail(s.adminsession,"ils -L "+datafilename,"LIST",datafilename) # should not be listed
        assertiCmd(s.adminsession,"iput "+datafilename) # iput
        assertiCmd(s.adminsession,"ils -L "+datafilename,"LIST",datafilename) # should be listed
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

    def test_local_iput_overwrite(self):
        assertiCmdFail(s.adminsession,"iput "+s.testfile) # fail, already exists
        assertiCmd(s.adminsession,"iput -f "+s.testfile) # iput again, force

    def test_local_iput_recursive(self):
        recursivedirname = "dir"

    def test_local_iput_checksum(self):
        # local setup
        datafilename = "newfile.txt"
        f = open(datafilename,'wb')
        f.write("TESTFILE -- ["+datafilename+"]")
        f.close()
        # assertions
        assertiCmd(s.adminsession,"iput -K "+datafilename) # iput
        assertiCmd(s.adminsession,"ils -L","LIST","d60af3eb3251240782712eab3d8ef3b1") # check proper checksum
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

    def test_local_iput_onto_specific_resource(self):
        # local setup
        datafilename = "anotherfile.txt"
        f = open(datafilename,'wb')
        f.write("TESTFILE -- ["+datafilename+"]")
        f.close()
        # assertions
        assertiCmdFail(s.adminsession,"ils -L "+datafilename,"LIST",datafilename) # should not be listed
        assertiCmd(s.adminsession,"iput -R testResc "+datafilename) # iput
        assertiCmd(s.adminsession,"ils -L "+datafilename,"LIST",datafilename) # should be listed
        assertiCmd(s.adminsession,"ils -L "+datafilename,"LIST","testResc") # should be listed
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

    def test_local_iput_interrupt_directory(self):
        # local setup
        datadir = "newdatadir"
        output = commands.getstatusoutput( 'mkdir '+datadir )
        datafiles = ["file1","file2","file3","file4","file5","file6","file7"]
        for datafilename in datafiles:
            print "-------------------"
            print "creating "+datafilename+"..."
            localpath = datadir+"/"+datafilename
            output = commands.getstatusoutput( 'dd if=/dev/zero of='+localpath+' bs=1M count=20' )
            print output[1]
            assert output[0] == 0, "dd did not successfully exit"
        rf="collectionrestartfile"
        # assertions
        iputcmd = "iput -X "+rf+" -r "+datadir
        if os.path.exists(rf): os.unlink(rf)
        interruptiCmd(s.adminsession,iputcmd,rf,10) # once restartfile reaches 10 bytes
        assert os.path.exists(rf), rf+" should now exist, but did not"
        output = commands.getstatusoutput( 'cat '+rf )
        print "  restartfile ["+rf+"] contents --> ["+output[1]+"]"
        assertiCmd(s.adminsession,"ils -L "+datadir,"LIST",datadir) # just to show contents
        assertiCmd(s.adminsession,iputcmd,"LIST","File last completed") # confirm the restart
        for datafilename in datafiles:
            assertiCmd(s.adminsession,"ils -L "+datadir,"LIST",datafilename) # should be listed
        # local cleanup
        output = commands.getstatusoutput( 'rm -rf '+datadir )
        output = commands.getstatusoutput( 'rm '+rf )

    def test_local_iput_interrupt_largefile(self):
        # local setup
        datafilename = "bigfile"
        print "-------------------"
        print "creating "+datafilename+"..."
        output = commands.getstatusoutput( 'dd if=/dev/zero of='+datafilename+' bs=1M count=150' )
        print output[1]
        assert output[0] == 0, "dd did not successfully exit"
        rf="bigrestartfile"
        # assertions
        iputcmd = "iput --lfrestart "+rf+" "+datafilename
        if os.path.exists(rf): os.unlink(rf)
        interruptiCmd(s.adminsession,iputcmd,rf,10) # once restartfile reaches 10 bytes
        assert os.path.exists(rf), rf+" should now exist, but did not"
        output = commands.getstatusoutput( 'cat '+rf )
        print "  restartfile ["+rf+"] contents --> ["+output[1]+"]"
        today = datetime.date.today()
        assertiCmd(s.adminsession,"ils -L "+datafilename,"LIST",[" 0 "+today.isoformat(),datafilename]) # will have zero length
        assertiCmd(s.adminsession,iputcmd,"LIST",datafilename+" was restarted successfully") # confirm the restart
        assertiCmdFail(s.adminsession,"ils -L "+datafilename,"LIST",[" 0 "+today.isoformat(),datafilename]) # length should not be zero
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )
        output = commands.getstatusoutput( 'rm '+rf )

    def test_local_iput_physicalpath_no_permission(self):
        # local setup
        datafilename = "newfile.txt"
        f = open(datafilename,'wb')
        f.write("TESTFILE -- ["+datafilename+"]")
        f.close()
        # assertions
        assertiCmd(s.adminsession,"iput -p /newfileinroot.txt "+datafilename,"ERROR",["UNIX_FILE_CREATE_ERR","Permission denied"]) # should fail to write
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

    def test_local_iput_physicalpath(self):
        # local setup
        datafilename = "newfile.txt"
        f = open(datafilename,'wb')
        f.write("TESTFILE -- ["+datafilename+"]")
        f.close()
        # assertions
        fullpath = "/var/lib/eirods/newphysicalpath.txt"
        assertiCmd(s.adminsession,"iput -p "+fullpath+" "+datafilename) # should complete
        assertiCmd(s.adminsession,"ils -L "+datafilename,"LIST",datafilename) # should be listed
        assertiCmd(s.adminsession,"ils -L "+datafilename,"LIST",fullpath) # should be listed
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

    def test_admin_local_iput_relative_physicalpath_into_server_bin(self):
        # local setup
        datafilename = "newfile.txt"
        f = open(datafilename,'wb')
        f.write("TESTFILE -- ["+datafilename+"]")
        f.close()
        # assertions
        relpath = "relativephysicalpath.txt"
        assertiCmd(s.adminsession,"iput -p "+relpath+" "+datafilename) # should allow put into server/bin
        assertiCmd(s.adminsession,"ils -L "+datafilename,"LIST",datafilename)
        assertiCmd(s.adminsession,"ils -L "+datafilename,"LIST",relpath)
        # NOTE: cannot remove the file from virtual filesystem without force flag
        #       because iRODS cannot create similar path in trash
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

    def test_local_iput_relative_physicalpath_into_server_bin(self):
        # local setup
        datafilename = "newfile.txt"
        f = open(datafilename,'wb')
        f.write("TESTFILE -- ["+datafilename+"]")
        f.close()
        # assertions
        relpath = "relativephysicalpath.txt"
        assertiCmd(s.sessions[1],"iput -p "+relpath+" "+datafilename,"ERROR","PATH_REG_NOT_ALLOWED") # should error
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

    def test_local_iput_with_changed_target_filename(self):
        # local setup
        datafilename = "newfile.txt"
        f = open(datafilename,'wb')
        f.write("TESTFILE -- ["+datafilename+"]")
        f.close()
        # assertions
        changedfilename = "different.txt"
        assertiCmd(s.adminsession,"iput "+datafilename+" "+changedfilename) # should complete
        assertiCmd(s.adminsession,"ils -L "+changedfilename,"LIST",changedfilename) # should be listed
        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

    def test_local_iput_collision_with_wlock(self):
        # local setup
        datafilename = "collisionfile1.txt"
        print "-------------------"
        print "creating "+datafilename+"..."
        output = commands.getstatusoutput( 'dd if=/dev/zero of='+datafilename+' bs=1M count=30' )
        print output[1]
        assert output[0] == 0, "dd did not successfully exit"
        # assertions

        begin = time.time()
        errorflag = False

        procs = set()
        pids = set()

        # start multiple icommands in parallel
        initialdelay = 3  # seconds
        for i in range(5):
            if i == 0:
                # add a three second delay before the first icommand
                p = s.adminsession.runCmd('iput',["-vf","--wlock",datafilename],waitforresult=False,delay=initialdelay)
            else:
                p = s.adminsession.runCmd('iput',["-vf","--wlock",datafilename],waitforresult=False,delay=0)
            procs.add(p)
            pids.add(p.pid)

        while pids:
            pid,retval=os.wait()
            for proc in procs:
                if proc.pid == pid:
                    print "pid "+str(pid)+":"
                    if retval != 0:
                        print "  * ERROR occurred *  <------------"
                        errorflag = True
                    print "  retval ["+str(retval)+"]"
                    print "  stdout ["+proc.stdout.read().strip()+"]"
                    print "  stderr ["+proc.stderr.read().strip()+"]"
                    pids.remove(pid)

        elapsed = time.time() - begin
        print "\ntotal time ["+str(elapsed)+"]"

        assert elapsed > initialdelay

        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

        assert errorflag == False, "oops, had an error"

    def test_local_iput_collision_without_wlock(self):
        # local setup
        datafilename = "collisionfile2.txt"
        print "-------------------"
        print "creating "+datafilename+"..."
        output = commands.getstatusoutput( 'dd if=/dev/zero of='+datafilename+' bs=1M count=30' )
        print output[1]
        assert output[0] == 0, "dd did not successfully exit"
        # assertions

        begin = time.time()
        errorflag = False

        procs = set()
        pids = set()

        # start multiple icommands in parallel
        initialdelay = 3  # seconds
        for i in range(7):
            if i == 0:
                # add a three second delay before the first icommand
                p = s.adminsession.runCmd('iput',["-vf",datafilename],waitforresult=False,delay=initialdelay)
            else:
                p = s.adminsession.runCmd('iput',["-vf",datafilename],waitforresult=False,delay=0)
            procs.add(p)
            pids.add(p.pid)

        while pids:
            pid,retval=os.wait()
            for proc in procs:
                if proc.pid == pid:
                    print "pid "+str(pid)+":"
                    if retval != 0:
                        errorflag = True
                    else:
                        print "  * Unexpectedly, No ERROR occurred *  <------------"
                    print "  retval ["+str(retval)+"]"
                    print "  stdout ["+proc.stdout.read().strip()+"]"
                    print "  stderr ["+proc.stderr.read().strip()+"]"
                    pids.remove(pid)

        elapsed = time.time() - begin
        print "\ntotal time ["+str(elapsed)+"]"

        # local cleanup
        output = commands.getstatusoutput( 'rm '+datafilename )

        assert errorflag == True, "Expected ERRORs did not occur"

    ###################
    # ireg
    ###################

    def test_ireg_as_rodsadmin(self):
        # local setup
        filename = "newfile.txt"
        filepath = os.path.abspath(filename)
        f = open(filepath,'wb')
        f.write("TESTFILE -- ["+filepath+"]")
        f.close()

        # keyword arguments for icommands
        kwargs={'filepath':filepath, 
            'filename':filename, 
            'zone':s.adminsession.getZoneName(),
            'username':s.adminsession.getUserName(),
            'sessionId':s.adminsession.sessionId}

        # assertions
        assertiCmdFail(s.adminsession,"ils -L {filename}".format(**kwargs),"LIST",filename) # should not be listed
        assertiCmd(s.adminsession,"ireg {filepath} /{zone}/home/{username}/{sessionId}/{filename}".format(**kwargs)) # ireg
        assertiCmd(s.adminsession,"ils -L {filename}".format(**kwargs),"LIST",filename) # should be listed

        # local cleanup
        output = commands.getstatusoutput( 'rm '+filepath )

    def test_ireg_as_rodsuser(self):
        # local setup
        filename = "newfile.txt"
        filepath = os.path.abspath(filename)
        f = open(filepath,'wb')
        f.write("TESTFILE -- ["+filepath+"]")
        f.close()

        # keyword arguments for icommands
        kwargs={'filepath':filepath, 
            'filename':filename, 
            'zone':s.sessions[1].getZoneName(),
            'username':s.sessions[1].getUserName(),
            'sessionId':s.sessions[1].sessionId}

        # assertions
        assertiCmdFail(s.sessions[1],"ils -L {filename}".format(**kwargs),"LIST",filename) # should not be listed
        assertiCmd(s.sessions[1],"ireg {filepath} /{zone}/home/{username}/{sessionId}/{filename}".format(**kwargs), "ERROR","SYS_NO_PATH_PERMISSION") # ireg
        assertiCmdFail(s.sessions[1],"ils -L {filename}".format(**kwargs),"LIST",filename) # should not be listed

        # local cleanup
        output = commands.getstatusoutput( 'rm '+filepath )

    ###################
    # irm
    ###################

    def test_irm_doesnotexist(self):
        assertiCmdFail(s.adminsession,"irm doesnotexist") # does not exist

    def test_irm(self):
        assertiCmd(s.adminsession,"ils -L "+s.testfile,"LIST",s.testfile) # should be listed
        assertiCmd(s.adminsession,"irm "+s.testfile) # remove from grid
        assertiCmdFail(s.adminsession,"ils -L "+s.testfile,"LIST",s.testfile) # should be deleted
        trashpath = "/"+s.adminsession.getZoneName()+"/trash/home/"+s.adminsession.getUserName()+"/"+s.adminsession.sessionId
        assertiCmd(s.adminsession,"ils -L "+trashpath+"/"+s.testfile,"LIST",s.testfile) # should be in trash

    def test_irm_force(self):
        assertiCmd(s.adminsession,"ils -L "+s.testfile,"LIST",s.testfile) # should be listed
        assertiCmd(s.adminsession,"irm -f "+s.testfile) # remove from grid
        assertiCmdFail(s.adminsession,"ils -L "+s.testfile,"LIST",s.testfile) # should be deleted
        trashpath = "/"+s.adminsession.getZoneName()+"/trash/home/"+s.adminsession.getUserName()+"/"+s.adminsession.sessionId
        assertiCmdFail(s.adminsession,"ils -L "+trashpath+"/"+s.testfile,"LIST",s.testfile) # should not be in trash

    def test_irm_specific_replica(self):
        assertiCmd(s.adminsession,"ils -L "+s.testfile,"LIST",s.testfile) # should be listed
        assertiCmd(s.adminsession,"irepl -R "+s.testresc+" "+s.testfile) # creates replica
        assertiCmd(s.adminsession,"ils -L "+s.testfile,"LIST",s.testfile) # should be listed twice
        assertiCmd(s.adminsession,"irm -n 0 "+s.testfile) # remove original from grid
        assertiCmd(s.adminsession,"ils -L "+s.testfile,"LIST",["1 "+s.testresc,s.testfile]) # replica 1 should be there
        assertiCmdFail(s.adminsession,"ils -L "+s.testfile,"LIST",["0 "+s.adminsession.getDefResource(),s.testfile]) # replica 0 should be gone
        trashpath = "/"+s.adminsession.getZoneName()+"/trash/home/"+s.adminsession.getUserName()+"/"+s.adminsession.sessionId
        assertiCmdFail(s.adminsession,"ils -L "+trashpath+"/"+s.testfile,"LIST",["0 "+s.adminsession.getDefResource(),s.testfile]) # replica should not be in trash

    def test_irm_recursive_file(self):
        assertiCmd(s.adminsession,"ils -L "+s.testfile,"LIST",s.testfile) # should be listed
        assertiCmd(s.adminsession,"irm -r "+s.testfile) # should not fail, even though a collection

    def test_irm_recursive(self):
        assertiCmd(s.adminsession,"icp -r "+s.testdir+" copydir") # make a dir copy
        assertiCmd(s.adminsession,"ils -L ","LIST","copydir") # should be listed
        assertiCmd(s.adminsession,"irm -r copydir") # should remove
        assertiCmdFail(s.adminsession,"ils -L ","LIST","copydir") # should not be listed

    def test_irm_with_read_permission(self):
        assertiCmd(s.sessions[1],"icd ../../public") # switch to shared area
        assertiCmd(s.sessions[1],"ils -AL "+s.testfile,"LIST",s.testfile) # should be listed
        assertiCmdFail(s.sessions[1],"irm "+s.testfile) # read perm should not be allowed to remove
        assertiCmd(s.sessions[1],"ils -AL "+s.testfile,"LIST",s.testfile) # should still be listed

    def test_irm_with_write_permission(self):
        assertiCmd(s.sessions[2],"icd ../../public") # switch to shared area
        assertiCmd(s.sessions[2],"ils -AL "+s.testfile,"LIST",s.testfile) # should be listed
        assertiCmdFail(s.sessions[2],"irm "+s.testfile) # write perm should not be allowed to remove
        assertiCmd(s.sessions[2],"ils -AL "+s.testfile,"LIST",s.testfile) # should still be listed

    ###################
    # irmtrash
    ###################

    def test_irmtrash_admin(self):
        # assertions
        assertiCmd(s.adminsession,"irm "+s.testfile) # remove from grid
        assertiCmd(s.adminsession,"ils -rL /"+s.adminsession.getZoneName()+"/trash/home/"+s.adminsession.getUserName()+"/","LIST",s.testfile) # should be listed
        assertiCmd(s.adminsession,"irmtrash") # should be listed
        assertiCmdFail(s.adminsession,"ils -rL /"+s.adminsession.getZoneName()+"/trash/home/"+s.adminsession.getUserName()+"/","LIST",s.testfile) # should be deleted
