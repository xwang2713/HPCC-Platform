## Implementation
### Current method
Faraget require special VPC 
Currently I use eskctl with "--fargate" to generate it. I create a EFS and set network private subnets to the EFS. Create a EFS mount point (default /).
I can't keep the VPC since delete the EFS/Fargate will remove at lest part of VPC subnets even EFS hold the private subnets.

I am using ADFS which has ADFS-Developer role 
Our hpcc.build user probably missing fargatePodExecutionRole

## Reference
https://docs.aws.amazon.com/AmazonECS/latest/developerguide/ECS_AWSCLI_Fargate.html#ECS_AWSCLI_Fargate_create_cluster
/home/ming/work/HPCC/HPCC-Kube/HPCC-Kube/aws/EKS/Fargate

https://docs.aws.amazon.com/efs/latest/ug/efs-access-points.html#enforce-identity-access-points

### Prerequisites
https://docs.aws.amazon.com/AmazonECS/latest/developerguide/security_iam_id-based-policy-examples.html#first-run-permissions
1. AmazonECS_FullAccess
2. AmazonEKSFargatePodExecutionRole

## Some Reference from AWS Support
1. https://repost.aws/knowledge-center/eks-troubleshoot-efs-volume-mount-issues
2. https://github.com/kubernetes-sigs/aws-efs-csi-driver/tree/master/examples/kubernetes/access_points
3. https://raw.githubusercontent.com/kubernetes-sigs/aws-efs-csi-driver/master/examples/kubernetes/statefulset/specs/example.yaml


## Developer Role
```console
aws iam get-role --role-name ADFS-Developer
ROLE    arn:aws:iam::446598291512:role/ADFS-Developer   2021-09-16T19:53:16+00:00       28800   /       AROAWP62VEA4H3GCSE3OZ   ADFS-Developer
ASSUMEROLEPOLICYDOCUMENT        2012-10-17
STATEMENT       sts:AssumeRoleWithSAML  Allow
STRINGEQUALS    https://signin.aws.amazon.com/saml
PRINCIPAL       arn:aws:iam::446598291512:saml-provider/ADFS
```

## Create/Delete
### Create EKS with Fargate enable
https://codersociety.com/blog/articles/kubernetes-eks-fargate
```console
./create-eks-fargate.sh
kubectl get node
NAME                                                    STATUS   ROLES    AGE   VERSION
fargate-ip-192-168-109-137.us-east-2.compute.internal   Ready    <none>   87s   v1.22.17-eks-38d8db8
fargate-ip-192-168-147-119.us-east-2.compute.internal   Ready    <none>   85s   v1.22.17-eks-38d8db8
```

### Delete
```console
./delete-eks-fargate.sh
```

## EFS
### Create EFS with VPC/Subnets from above AKS/Fargate
```code
1. Select PVC used in above EKS/Fargate
2. Select private subnet in "Network Access" "Subnet ID" fields
```
### EFS for Fargate
fs-0b9983807d1a9f3bf

### Set access point
```code
1. Name: hpcc-data (optional)
2. Root directory path: /
3. Tags:  admin=ming email=<your email>
```

### EFS tutorial
https://www.youtube.com/watch?v=4MjbKDBkOdE


## CSI driver
https://aws.amazon.com/about-aws/whats-new/2020/08/amazon-ek-on-aws-fargate-now-supports-amazon-efs-file-systems/

### error
```console
Error: creating OIDC provider: operation error IAM: CreateOpenIDConnectProvider, https response error StatusCode: 403, RequestID: c6077aae-fc3d-4021-9b89-c1cf191f8814, api error AccessDenied: User: arn:aws:sts::446598291512:assumed-role/ADFS-HPCCPlatformTeam/WangXi01@risk.regn.net is not authorized to perform: iam:TagOpenIDConnectProvider on resource: arn:aws:iam::446598291512:oidc-provider/oidc.eks.us-east-2.amazonaws.com because no identity-based policy allows the iam:TagOpenIDConnectProvider action
install then amazon efs driver
```

Also get following error when run script create-oidc-provider.sh
```console
```
https://docs.aws.amazon.com/cli/latest/reference/iam/put-user-policy.html#put-user-policy

## Fargate
### Get Fargate Profile
```code
eksctl get fargateprofile --cluster hpcc-fargate-ming --region us-east-2
```

## Load-Balancer-Controller
### Install
in ./load/balancer/
run ./lbc.sh
### Delete
Not sure helm delete enough or not
Otherwise go to EC2 service and Load Balancer to delete it
Withtout is VPC and Subnets delete will fail

## AWS Support
### EFS Mount

[CASE 13067780701] Cannot mount EFS/CSI share with EKS/Fargate
```code
This is Haithem again, thank you for your patience while I was replicating this issue. I was able to get the same error as you:

Warning FailedMount 5s (x5 over 14s) kubelet MountVolume.SetUp failed for volume "efs-pv" : rpc error: code = Internal desc = Could not mount "fs-xxxxxxxxxxx:/foobar" at "/var/lib/kubelet/pods/9dc7521f-042a-4dfd-859e-163f537ce4f5/volumes/kubernetes.io~csi/efs-pv/mount": mount failed: exit status 32
Mounting command: mount
Mounting arguments: -t efs -o accesspoint=fsap-XXXXXXXXXXXX,tls fs-XXXXXXXXXXXXXXXXXXX:/foobar /var/lib/kubelet/pods/9dc7521f-042a-4dfd-859e-163f537ce4f5/volumes/kubernetes.io~csi/efs-pv/mount
Output: Could not start amazon-efs-mount-watchdog, unrecognized init system "bash"
b'mount.nfs4: mounting 127.0.0.1:/foobar failed, reason given by server: No such file or directory'
Warning: config file does not have fips_mode_enabled item in section mount.. You should be able to find a new config file in the same folder as current config file /etc/amazon/efs/efs-utils.conf. Consider update the new config file to latest config file. Use the default value [fips_mode_enabled = False].Warning: config file does not have retry_nfs_mount_command item in section mount.. You should be able to find a new config file in the same folder as current config file /etc/amazon/efs/efs-utils.conf. Consider update the new config file to latest config file. Use the default value [retry_nfs_mount_command = True].


Before getting into how I fixed this issue, here is how I have replicated the issue:

1. I have created an EFS volume and have created an Access Point with the name /foobar
2. I made sure that the security group attached to my EFS volume had the inbound rule TCP/2049
3. I did spin a quick EC2 instance and tried to connect to the Access point using the following command `sudo mount -t efs -o tls,accesspoint=fsap-02aa549578a12d2fb fs-0f0e45fb5be921aa3:/ /foobar` (I have created the /foobar directory on my instance and I also have installed amazon-efs-utils)
4. I deployed the following manifest (I have redacted some information as it does have my EFS volume and other sensitive data 



---
kind: StorageClass
apiVersion: storage.k8s.io/v1
metadata:
name: efs-sc
provisioner: efs.csi.aws.com
---
apiVersion: v1
kind: PersistentVolume
metadata:
name: efs-pv
spec:
capacity:
storage: 5Gi
volumeMode: Filesystem
accessModes:
- ReadWriteMany
persistentVolumeReclaimPolicy: Retain
storageClassName: efs-sc
mountOptions:
- tls
csi:
driver: efs.csi.aws.com
volumeHandle: fs-0f0e45fb5be921aa3::fsap-02aa549578a12d2fb
---
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
name: efs-claim
spec:
accessModes:
- ReadWriteMany
storageClassName: efs-sc
resources:
requests:
storage: 5Gi
---
apiVersion: v1
kind: Pod
metadata:
name: efs-app
spec:
containers:
- name: app
image: ubuntu
command: ['sh', '-c', 'echo "Hello, Kubernetes!" && sleep 3600']
volumeMounts:
- name: efs-volume
mountPath: /foobar
volumes:
- name: efs-volume
persistentVolumeClaim:
claimName: efs-claim


5. After deploying the above manifest I was able to exec to my Ubuntu pod and I was able to access my EFS

on the PersistentVolume if I use the following VolumeHandle the above error is thrown:

volumeHandle: fs-FileSystemId:/foobar:fsap-AccessPointId 

Setting it to `volumeHandle: [FileSystemId]::[AccessPointId]`[1] seems to be the only way to get it to work on either Fargate or non Fargate EKS cluster, please note that according to the same document [1] it does mention that the [Subpath] must already exist prior to consuming the volume from a pod.

6. Finally, after getting my Ubuntu pod to work and connect to the EFS access point I was able to write to the volume and the new files were visible from my EC2 instance as well which is not part of neither EKS clusters.

I hope this information I provided is helpful. If you require any further assistance please don't hesitate to reach out.

```
I tried this for ubuntu2204
```code
Follow this to install amazon-efs-utils: https://docs.aws.amazon.com/efs/latest/ug/installing-amazon-efs-utils.html#installing-other-distro
apt install -y vim
add "region=us-east-2" under [default] and [mount] in /etc/amazon/efs/efs-utils.conf
```


127.0.0.1:/ on /var/lib/HPCCSystems/queries type nfs4 (rw,relatime,vers=4.1,rsize=1048576,wsize=1048576,namlen=255,hard,noresvport,proto=tcp,port=20186,timeo=600,retrans=2,sec=sys,clientaddr=127.0.0.1,local_lock=none,addr=127.0.0.1)

To resolve it use format:
```code
volumeHandle: <efsId>:/<subPath>
```
But above probably will get error. To workaround it use following for the access point first
```code
volumeHandle: <efsId>::<apId>
```
Reference this for detail: https://docs.aws.amazon.com/efs/latest/ug/efs-access-points.html

### Access Fargate External Address
```code
I replicated the same environment on my Lab, and checked using "kubectl port-forward name-of-pod 8080:80" that the nginx container is running well, however using the dns provided by the LoadBalancer it's throwing an empty response.

To solve the issue, we have two choices, both require installing the "AWS Load Balancer Controller" in our cluster using Helm.

1. Either we can use Ingress to expose the nginx workload, please check the following link which has some requirements before implementing that:
https://archive.eksworkshop.com/beginner/180_fargate/prerequisites-for-alb/
Or
https://faun.pub/playing-with-eks-fargate-21bb02e546e4
2. Or use the NLB IP Targetting, which involves adding annotations to our service and installing the ALB Controller in the cluster. The following link includes all the prerequisites plus a sample app to test the feature:
https://docs.aws.amazon.com/eks/latest/userguide/network-load-balancing.html

```
Will try https://archive.eksworkshop.com/beginner/180_fargate/prerequisites-for-alb/

