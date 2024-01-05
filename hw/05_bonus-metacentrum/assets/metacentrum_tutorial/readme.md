# MetaCentrum tutorial
In the document, `<username>` is your username on the MetaCentrum.

Copy the files to metacentrum
```bash
scp main.cpp <username>@skirit.metacentrum.cz:/storage/brno2/home/<username>
scp runner.sh <username>@skirit.metacentrum.cz:/storage/brno2/home/<username>
```

Connect to metacentrum
```bash
ssh <username>@skirit.metacentrum.cz
```

Run job
```bash
qsub -l select=1:ncpus=4:mem=4gb:scratch_local=10gb -l walltime=00:05:00 runner.sh
```

Query your jobs
```bash
qstat -xu $LOGNAME
```

Kill a job, where `<id>` is the id of the job returned by qsub
```bash
qdel <id>
```

For the official MetaCentrum quickstart + pointers to details, visit
https://wiki.metacentrum.cz/wiki/How_to_compute/Quick_start

## FAQ

1. **My job has status `M`, what does it mean?**
Your job was moved from one queue to another.
To query your jobs over all queues you can call

```bash
qstat -xu $LOGNAME @pbs.elixir-czech.cz @arien-pro.ics.muni.cz @wagap-pro.cerit-sc.cz
```

2. **How to obtain the hardware information about the machine where my job was executed?**
Inside your `runner.sh`, obtain the machine hostname by Linux command `hostname` and find it at https://metavo.metacentrum.cz/pbsmon2/nodes/physical .
For example, if `hostname` gives you `luna24.fzu.cz`, then your job was executed on node `luna24` of the `luna.fzu.cz` cluster.
Alternatively, for already finished jobs you can query the full job information by
```bash
qstat -xf <id>
```
where `<id>` is the job id.
The name of the executing machine is under `exec_host` key.

3. **How to use MPI/OpenMP?**: Check https://wiki.metacentrum.cz/wiki/Jak_po%C4%8D%C3%ADtat/Paralelizace .
For OpenMP, make sure that `$OMP_NUM_THREADS` environment variable is set and its value is at most `$PBS_NUM_PPN`.