import os
import optparse, sys
sys.path.insert(0, "/afs/cern.ch/work/v/vveckaln/private/CMSSW_8_0_26_patch1/src/TopLJets2015/TopAnalysis/condor/")
try:
    import testfilesanity
except ImportError:
    print ('No Import')
EOS=os.getenv('EOS')
PROJECT="/afs/cern.ch/work/v/vveckaln/private/UnfoldingLIB" 
FarmOutputDirectory = '%s/condor/CONDOROUT' % PROJECT
FarmCfgDirectory = '%s/condor/FARM' % PROJECT

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-b', '--bins',           action = 'store_true',      dest = 'bins',            default = False,     help = 'calculate bins')
parser.add_option(      '--binning_method', type = 'string',            dest = 'binning_method',  default = '',        help = 'binning method')
parser.add_option(      '--sfactor',        type = 'float',             dest = 'sfactor',         default = 0.0,       help = 'binning method')
parser.add_option('-m', '--method',         type = 'string',            dest = 'method',          default = "nominal", help = 'nominal or tailor made implementation')
parser.add_option('-n',                     action = 'store_true',      dest = 'presampleTTJets', default = False,     help = 'fill signal histos for TTJets')
parser.add_option(      '--observable',     type = 'string',            dest = 'observable',      default = '',        help = 'observable')

(opt, args)    = parser.parse_args()
observable     = opt.observable
bins           = opt.bins
method         = opt.method
binning_method = opt.binning_method
sfactor        = opt.sfactor
#samplelist           = ['MC13TeV_TTJets', 'MC13TeV_TTJets_herwig', 'MC13TeV_TTJets2l2nu_amcatnlo']
samplelist = 0
chargelist           = [0, 1]
jetlist              = [0, 1, 2, 3]
jettags              = ['leading_jet', 'scnd_leading_jet', 'leading_b', 'scnd_leading_b']
chargetags           = ['allconst', 'chconst']
levellist            = 0
OutputDirectory      = 0

if bins:
    OutputDirectory = '%s/bins/%s/SIGMA_%s' % (EOS, observable, str(sfactor).replace(".", "p"))
    OutputDirectory.replace(".", "p")
    levellist = [0]
    samplelist           = ['MC13TeV_TTJets']
else:
#    levellist = [0, 1]
    levellist = [1]
    if method == 'nominal':
        if opt.presampleTTJets:
            samplelist           = ['MC13TeV_TTJets']
        else:
            samplelist           = ['MC13TeV_TTJets_herwig']
            
    if method == 'cflip':
        if opt.presampleTTJets:
            samplelist           = ['MC13TeV_TTJets']
        else:
            samplelist           = ['MC13TeV_TTJets_cflip']

leveltags            = ['ORIG', 'OPT']
os.system('mkdir -p %s' % FarmOutputDirectory)
os.system('mkdir -p %s' % FarmCfgDirectory)
print 'Preparing tasks to submit to the batch'
print 'Executables and condor wrapper are stored in %s' % FarmCfgDirectory
with open ('%s/condor.sub' % FarmCfgDirectory, 'w') as condor:

    condor.write('executable  = {0}/$(cfgFile).sh\n'.format(FarmCfgDirectory))
    condor.write('output      = {0}/$(cfgFile).out\n'.format(FarmOutputDirectory))
    condor.write('error       = {0}/$(cfgFile).err\n'.format(FarmOutputDirectory))
    condor.write('log         = {0}/*.$(ClusterId).log\n'.format(FarmCfgDirectory))
    condor.write('arguments   = $(ClusterId) $(ProcId)\n')
    condor.write('+JobFlavour = "{0}"\n'.format("workday"))

    njob = 0
    sjobs = 0
    for jetind in jetlist:
        for chargeind in chargelist:
            for sampletag in samplelist:
                for levelind in levellist:
                    njob += 1
                    binmethodtag         = ""
                    binsubdir            = ""
                    if levelind == 0:
                        binsubdir = "ORIG"
                    if levelind == 1:
                        if binning_method == "SIGMA":
                            binsubdir = binning_method + "_" + str(sfactor).replace(".", "p")
                            binmethodtag += "_" + binsubdir
                        else:
                            binsubdir = binning_method
                            binmethodtag += "_" + binsubdir
                    job_tag = ""
                    if not bins:
                        OutputDirectory = '%s/unfolding_%s/%s/%s/' % (EOS, method, observable, binsubdir)
#                        os.system('mkdir -p %s' % OutputDirectory)
                        job_tag = jettags[jetind] + '_' + chargetags[chargeind] + '_' + sampletag + '_' + observable + '_' + leveltags[levelind] + '_' + method + binmethodtag
                    else:
                        job_tag = jettags[jetind] + '_' + chargetags[chargeind] + '_' + sampletag + '_' + observable + '_' + leveltags[levelind] + '_' + method + "_binSIGMA_" + str(sfactor).replace(".", "p")
#                        os.system('mkdir -p %s' % OutputDirectory)

                    print "********* njob %u job_tag %s **************" % (njob, job_tag)
                    if os.path.isdir(OutputDirectory + '/' + job_tag):
#                        res = testfilesanity.testsanity(OutputDirectory + '/' + job_tag + '/save.root')
                        if  testfilesanity.testsanity(OutputDirectory + '/' + job_tag + '/save.root') == 0:
                            print 'skipped %s' % job_tag
                            continue
                        else:
                            print 'bad file discovered %s' % (OutputDirectory + '/' + job_tag + '/save.root')
                            raw_input("penter")
                    else:
                        print 'file %s not found' % (OutputDirectory + '/' + job_tag + '/save.root')
                        raw_input("penter")
                    sjobs += 1
                    cfgFile = '%s' % job_tag
                    condor.write('cfgFile=%s\n' % cfgFile) 
                    condor.write('queue 1\n')
                    sedcmd  = 'sed \''
                    sedcmd += 's%@OUTPUTDIR%'            + OutputDirectory          + '%g;'
                    sedcmd += 's%@JOBTAG%'               + job_tag                  + '%g;'
                    sedcmd += 's%@JETIND%'               + str(jetind)              + '%g;'
                    sedcmd += 's%@CHARGEIND%'            + str(chargeind)           + '%g;'
                    sedcmd += 's%@SAMPLE%'               + sampletag                + '%g;'
                    sedcmd += 's%@OBSERVABLE%'           + observable               + '%g;'
                    sedcmd += 's%@LEVELIND%'             + str(levelind)            + '%g;'
                    sedcmd += 's%@METHOD%'               + str(method)              + '%g;'
                    sedcmd += 's%@PRESAMPLETTJETS%'      + str(opt.presampleTTJets) + '%g;'
                    sedcmd += 's%@BINNING_METHOD%'       + binning_method           + '%g;'
                    sedcmd += 's%@BINS%'                 + str(bins)                + '%g;'
                    sedcmd += 's%@SFACTOR%'              + str(sfactor)             + '%g;'
                    sedcmd += '\''
                
                    os.system('cat ' + PROJECT + '/condor/executable_templ.sh | ' + sedcmd + ' > condor/FARM/' + cfgFile + '.sh')
                
                    os.system('chmod u+x %s/%s.sh' % (FarmCfgDirectory, cfgFile))

print 'Submitting %u jobs to condor, flavour "%s"' % (sjobs, "longlunch")
os.system('condor_config_val -dump | grep CONDOR; condor_submit %s/condor.sub' % FarmCfgDirectory)
