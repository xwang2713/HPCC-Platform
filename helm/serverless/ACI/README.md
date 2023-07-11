
## Subscription and Quote

### AKS Virtual Node
We want AKS unlimited capacity and a per-second billing
virtual-kubelet.io/last-applied-node-status:
                      {"capacity":{"cpu":"10k","memory":"4Ti","pods":"5k"},"allocatable":{"cpu":"10k","memory":"4Ti","pods":"5k"},"conditions":[{"type":"Ready",...
                    virtual-kubelet.io/last-applied-object-meta:
                      {"name":"virtual-node-aci-linux","uid":"ab9308e3-2aef-486f-b03a-756e9c05e1fc","creationTimestamp":null,"labels":{"alpha.service-controller...

## Reference
### ACI
https://learn.microsoft.com/en-us/azure/container-instances/container-instances-quickstart

### General
https://learn.microsoft.com/en-us/azure/aks/virtual-nodes-portal
https://learn.microsoft.com/en-us/azure/aks/virtual-nodes

https://learn.microsoft.com/en-us/azure/aks/virtual-nodes-cli

https://blog.tomkerkhove.be/2021/01/02/autoscaling-azure-container-instances-with-azure-serverless/

https://kubernetes.io/blog/2022/08/29/csi-inline-volumes-ga/

Inline volume: https://learn.microsoft.com/en-us/azure/aks/azure-csi-files-storage-provision

https://bluexp.netapp.com/blog/azure-cvo-blg-azure-kubernetes-service-tutorial-integrate-aks-with-aci
https://www.youtube.com/watch?v=SKkJwr--mIg

### Scale up AKS with ACI
https://learn.microsoft.com/en-us/azure/architecture/solution-ideas/articles/scale-using-aks-with-aci

### AZ Cli
https://codersociety.com/blog/articles/kubernetes-aks-aci

### Inline Azurefile on ACI or AKS
https://learn.microsoft.com/en-us/azure/aks/azure-csi-files-storage-provision
https://learn.microsoft.com/en-us/azure/container-instances/container-instances-volume-azure-files
https://learn.microsoft.com/en-us/azure/container-instances/container-instances-volume-secret
https://zimmergren.net/mount-an-azure-storage-file-share-to-deployments-in-azure-kubernetes-services-aks/


### AKS + ACI example

https://bluexp.netapp.com/blog/azure-cvo-blg-azure-kubernetes-service-tutorial-integrate-aks-with-aci

### Scaling
https://github.com/Azure-Samples/virtual-node-autoscale

### Slow to start a pod/aci with ACI
https://learn.microsoft.com/en-us/azure/container-instances/container-instances-troubleshooting#container-takes-a-long-time-to-start
https://github.com/Azure/AKS/issues/2373

### Terraform
https://registry.terraform.io/modules/bcochofel/aks/azurerm/latest/examples/aci-connector

## Prerequitites
```console
 az provider list --query "[?contains(namespace,'Microsoft.ContainerInstance')]" -o table
Namespace                    RegistrationState    RegistrationPolicy
---------------------------  -------------------  --------------------
Microsoft.ContainerInstance  Registered           RegistrationRequired
```

## Create
### Create AKS/ACI with Azure Portal
```code
Must create Resource group first with tag "admin" and "email"
Resource group: rg-aci-test-ming
1. Kubernetes Services -> Create Kubernetes Cluster
2. Pick existing Resource group: rg-aci-test-ming
3. Kubernetes: aci-test-ming version 1-24-10
4. region: eastus zone: 1,2,3 
5. AKS price tie: standard
6. Nodeoool  DS2_v2 1-2 (system nodes)
7. Enable virtual nodes: true
8. Encrpytion: default
9. Tags
   market              = "us"
   admin               = "ming"
   email               = "xiaoming.wang@lexisnexisrisk.com"
   project             = "ACI Testing by Ming"
   location            = "eastus"
   sre_team            = "hpcc_platform"
   product_name        = "hpccsystems"
   business_unit       = "research and development"
   subscription_type   = "nonprod"

```
If it failed due to resource group it probably is 'Log Analytics Workspace" doesn't touch to existing resource group. You can create a new 'Log Analytics Workspace" with the existing resouce group

Login to Azure
```console
az login (make sure DNS working)
If get "invalid token" etc you may need to reboot the system or sync time:
sudo hwclock -s
```
Connect AKS: 
```console
az aks get-credentials --resource-group rg-aci-test-ming --name aci-test-ming
kubectl get nodes
NAME                                STATUS   ROLES   AGE   VERSION
aks-agentpool-35224751-vmss000000   Ready    agent   30m   v1.24.10
virtual-node-aci-linux              Ready    agent   29m   v1.19.10-vk-azure-aci-1.4.8
```
### Azure Container Registry
```code
Resource group: rg-acr-test-ming
Use tags as rg-aci-test-ming
Create Container Registries, for example acrhpccsystems (EASTUS, with necessory tags)
```
Enable pull images from this ACR:
```code
az acr update --name acrhpccsystems --anonymous-pull-enabled
```

Push an image to
```code
Get token:
az acr login -n acrhpccsystems --expose-token
You can perform manual login using the provided access token below, for example: 'docker login loginServer -u 00000000-0000-0000-0000-000000000000 -p accessToken'
{
  "accessToken": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6IkNPQVU6UERZSDo0SVJYOjM2SEI6TFYzUDpWNFBGOko0NzQ6SzNOSjpPS1JCOlRZQUo6NEc0Szo1Q1NEIn0.eyJqdGkiOiIxOGM3YmYwZC1hNGQyLTQxOTMtOTYyMS0zMGU4MzczOTA5OTAiLCJzdWIiOiJXYW5nWGkwMUByaXNrLnJlZ24ubmV0IiwibmJmIjoxNjg1MDc0ODQzLCJleHAiOjE2ODUwODY1NDMsImlhdCI6MTY4NTA3NDg0MywiaXNzIjoiQXp1cmUgQ29udGFpbmVyIFJlZ2lzdHJ5IiwiYXVkIjoiYWNyaHBjY3N5c3RlbXMuYXp1cmVjci5pbyIsInZlcnNpb24iOiIxLjAiLCJyaWQiOiIyYjJlZjk1ZTVmM2E0MjM2OTY3ZWFmMDAyYzRmNDFlNyIsImdyYW50X3R5cGUiOiJyZWZyZXNoX3Rva2VuIiwiYXBwaWQiOiIwNGIwNzc5NS04ZGRiLTQ2MWEtYmJlZS0wMmY5ZTFiZjdiNDYiLCJ0ZW5hbnQiOiJiYzg3N2U2MS1mNmNmLTQ0NjEtYWNjZC0wNTY1ZmE0Y2EzNTciLCJwZXJtaXNzaW9ucyI6eyJBY3Rpb25zIjpbInJlYWQiLCJ3cml0ZSIsImRlbGV0ZSIsImRlbGV0ZWQvcmVhZCIsImRlbGV0ZWQvcmVzdG9yZS9hY3Rpb24iXSwiTm90QWN0aW9ucyI6bnVsbH0sInJvbGVzIjpbXX0.lcdAJ-X80NS84XQGrU0YtrSmB2bqPx9t9SssHGuGN5AzLk1OzjOQy0uwN-nZDhw4opOyG5ST-AFVssYN94aksHem59qhhcMT9GwfKxyBtNOJ8cp8oadXZpa3_JLL6isAisUAMOgJo2dlvmwud8HwatldQ9qUCrwGWJBmWamsbfjOsT6OA57dwZG4aerRHOorDrWkF9TfkZ8-KIlmsOjVlGwQldBhdSzhsOUBHEHf2ptzevy2TRzanybFjtw25Cm20UwezdRS73F-JqA7D9TM_6skCpmFX7u8OcPn2K9teqQf7a5iMm7mxooQolSG8bb1q8JMEW99o88aOVEeZC2mkQ",
  "loginServer": "acrhpccsystems.azurecr.io"
}
```
Make sure the image is in local Docker:
```code
docker pull hpccsystems/platform-core:9.0.8
```
Login to ACR
```code
docker login acrhpccsystems.azurecr.io
Username: 00000000-0000-0000-0000-000000000000
Password: eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6IkNPQVU6UERZSDo0SVJYOjM2SEI6TFYzUDpWNFBGOko0NzQ6SzNOSjpPS1JCOlRZQUo6NEc0Szo1Q1NEIn0.eyJqdGkiOiIxOGM3YmYwZC1hNGQyLTQxOTMtOTYyMS0zMGU4MzczOTA5OTAiLCJzdWIiOiJXYW5nWGkwMUByaXNrLnJlZ24ubmV0IiwibmJmIjoxNjg1MDc0ODQzLCJleHAiOjE2ODUwODY1NDMsImlhdCI6MTY4NTA3NDg0MywiaXNzIjoiQXp1cmUgQ29udGFpbmVyIFJlZ2lzdHJ5IiwiYXVkIjoiYWNyaHBjY3N5c3RlbXMuYXp1cmVjci5pbyIsInZlcnNpb24iOiIxLjAiLCJyaWQiOiIyYjJlZjk1ZTVmM2E0MjM2OTY3ZWFmMDAyYzRmNDFlNyIsImdyYW50X3R5cGUiOiJyZWZyZXNoX3Rva2VuIiwiYXBwaWQiOiIwNGIwNzc5NS04ZGRiLTQ2MWEtYmJlZS0wMmY5ZTFiZjdiNDYiLCJ0ZW5hbnQiOiJiYzg3N2U2MS1mNmNmLTQ0NjEtYWNjZC0wNTY1ZmE0Y2EzNTciLCJwZXJtaXNzaW9ucyI6eyJBY3Rpb25zIjpbInJlYWQiLCJ3cml0ZSIsImRlbGV0ZSIsImRlbGV0ZWQvcmVhZCIsImRlbGV0ZWQvcmVzdG9yZS9hY3Rpb24iXSwiTm90QWN0aW9ucyI6bnVsbH0sInJvbGVzIjpbXX0.lcdAJ-X80NS84XQGrU0YtrSmB2bqPx9t9SssHGuGN5AzLk1OzjOQy0uwN-nZDhw4opOyG5ST-AFVssYN94aksHem59qhhcMT9GwfKxyBtNOJ8cp8oadXZpa3_JLL6isAisUAMOgJo2dlvmwud8HwatldQ9qUCrwGWJBmWamsbfjOsT6OA57dwZG4aerRHOorDrWkF9TfkZ8-KIlmsOjVlGwQldBhdSzhsOUBHEHf2ptzevy2TRzanybFjtw25Cm20UwezdRS73F-JqA7D9TM_6skCpmFX7u8OcPn2K9teqQf7a5iMm7mxooQolSG8bb1q8JMEW99o88aOVEeZC2mkQ
Login Succeeded
```

Tag image and push to ACR
```code
docker tag hpccsystems/platform-core:9.0.8 acrhpccsystems.azurecr.io/platform-core:9.0.8
docker push acrhpccsystems.azurecr.io/platform-core:9.0.8
az acr repository list --name acrhpccsystems --output table
az acr repository show-tags --name acrhpccsystems --repository platform-core --output table
```



## Current issues
### virtual-node-aci-linux doesn't displayed with kubectl get nodes
### PV/PVC non-support 
From Azure Portal we can start an AKS cluster with ACI. I try to deploy HPCC cluster but get error:
```code
 Warning  ProviderCreateFailed  49s (x14 over 2m46s)  virtual-node-aci-linux/pod-controller  pod mydali-75d65b5db4-js8ts requires volume dali-pv which is of an unsupported type
```
Form online discussion seem PV/PVC with ACI is not supported
https://github.com/MicrosoftDocs/azure-docs/issues/54443

### New Azure Container Instances quotes


### Most pod in pending states
1. AKS created from Azure Por2715tal
```code
   aks-agentpool-10082457-vmss000000   Ready    agent   25m   v1.24.10          100m (0%)     100m (0%)   400Mi (0%)       400Mi (0%)     6m30s
   virtual-node-aci-linux              Ready    agent   24m   v1.19.10-vk-azure-aci-1.4.
```  

2. Create storage account and shares
```code
   ./list-sa.sh
   ming@RIS-Cfpgz3T8FNL:~/work/HPCC/Cloud/Azure/ACI/HPCC-Platform/helm/examples/azure/sa$ ./list-sa.sh
[ Normal  RegisteredNode  16m   node-controller  Node virtual-node-aci-linux event: Registered Node virtual-node-aci-linux in Controller
  {g@RIS-Cfpgz3T8FNL:~/work/HPCC/Cloud/Azure/ACI/HPCC-Platform/helm$
    "accessTier": "TransactionOptimized",
    "deleted": null,
    "deletedTime": null,
    "etag": "\"0x8DB3C7374AE7B8E\"",
    "lastModified": "2023-04-13T23:04:37+00:00",
    "lease": {
      "duration": null,
      "state": "available",
      "status": "unlocked"
    },
    "metadata": null,
    "name": "dalishare",
    "nextAllowedQuotaDowngradeTime": null,
    "protocols": null,
    "provisionedBandwidth": null,
    "provisionedEgressMbps": null,
    "provisionedIngressMbps": null,
    "provisionedIops": null,
    "quota": 5120,
    "remainingRetentionDays": null,
    "rootSquash": null,
    "snapshot": null,
    "version": null
  },
  {
    "accessTier": "TransactionOptimized",
    "deleted": null,
    "deletedTime": null,
    "etag": "\"0x8DB3C73774093C0\"",
    "lastModified": "2023-04-13T23:04:42+00:00",
    "lease": {
      "duration": null,
      "state": "available",
      "status": "unlocked"
    },
    "metadata": null,
    "name": "datashare",
    "nextAllowedQuotaDowngradeTime": null,
    "protocols": null,
    "provisionedBandwidth": null,
    "provisionedEgressMbps": null,
    "provisionedIngressMbps": null,
    "provisionedIops": null,
    "quota": 5120,
    "remainingRetentionDays": null,
    "rootSquash": null,
    "snapshot": null,
    "version": null
  },
  {
    "accessTier": "TransactionOptimized",
    "deleted": null,
    "deletedTime": null,
    "etag": "\"0x8DB3C737589C883\"",
    "lastModified": "2023-04-13T23:04:39+00:00",
    "lease": {
      "duration": null,
      "state": "available",
      "status": "unlocked"
    },
    "metadata": null,
    "name": "dllsshare",
    "nextAllowedQuotaDowngradeTime": null,
    "protocols": null,
    "provisionedBandwidth": null,
    "provisionedEgressMbps": null,
    "provisionedIngressMbps": null,
    "provisionedIops": null,
    "quota": 5120,
    "remainingRetentionDays": null,
    "rootSquash": null,
    "snapshot": null,
    "version": null
  },
  {
    "accessTier": "TransactionOptimized",
    "deleted": null,
    "deletedTime": null,
    "etag": "\"0x8DB3C737818A01D\"",
    "lastModified": "2023-04-13T23:04:43+00:00",
    "lease": {
      "duration": null,
      "state": "available",
      "status": "unlocked"
    },
    "metadata": null,
    "name": "lzshare",
    "nextAllowedQuotaDowngradeTime": null,
    "protocols": null,
    "provisionedBandwidth": null,
    "provisionedEgressMbps": null,
    "provisionedIngressMbps": null,
    "provisionedIops": null,
    "quota": 5120,
    "remainingRetentionDays": null,
    "rootSquash": null,
    "snapshot": null,
    "version": null
  },
  {
    "accessTier": "TransactionOptimized",
    "deleted": null,
    "deletedTime": null,
    "etag": "\"0x8DB3C737663ED87\"",
    "lastModified": "2023-04-13T23:04:40+00:00",
    "lease": {
      "duration": null,
      "state": "available",
      "status": "unlocked"
    },
    "metadata": null,
    "name": "sashashare",
    "nextAllowedQuotaDowngradeTime": null,
    "protocols": null,
    "provisionedBandwidth": null,
    "provisionedEgressMbps": null,
    "provisionedIngressMbps": null,
    "provisionedIops": null,
    "quota": 5120,
    "remainingRetentionDays": null,
    "rootSquash": null,
    "snapshot": null,
    "version": null
  }
]
```
3. Deploy HPCC cluster
```code
hpcc install hpcc hpcc/ --set global.image.version=9.0.0
kubectl get pods
NAME                                          READY   STATUS    RESTARTS   AGE
dfs-5866696bbc-67zsj                          0/1     Pending   0          17m
dfuserver-55fc6f6dc7-sdcs5                    0/1     Pending   0          17m
eclqueries-f94db687b-n7l9h                    0/1     Pending   0          17m
eclscheduler-bf98ffcff-8kxqn                  1/1     Running   0          17m
eclservices-55584cbb86-zg65k                  0/1     Pending   0          17m
eclwatch-c7cdbb6d5-cxbs2                      0/1     Pending   0          17m
esdl-sandbox-74d875bf46-lj947                 0/1     Pending   0          17m
hthor-65b4b7b5b6-zfwzc                        0/1     Pending   0          17m
mydali-7dcf474db9-z7sn5                       0/2     Pending   0          17m
myeclccserver-bbbf6c997-8425k                 0/1     Pending   0          17m
roxie-agent-1-848bdb4498-6zgtg                0/1     Pending   0          17m
roxie-agent-1-848bdb4498-cdvdg                0/1     Pending   0          17m
roxie-agent-2-6748f799f7-b824h                0/1     Pending   0          17m
roxie-agent-2-6748f799f7-lmfkl                0/1     Pending   0          17m
roxie-toposerver-555f54544d-fd4fh             1/1     Running   0          17m
roxie-workunit-699698c9c5-n52g4               0/1     Pending   0          17m
sasha-dfurecovery-archiver-66c9ff598f-d4v7f   0/1     Pending   0          17m
sasha-dfuwu-archiver-67d684995b-dpmbw         0/1     Pending   0          17m
sasha-file-expiry-59fb5f4884-cks7b            0/1     Pending   0          17m
sasha-wu-archiver-6b757fdd6-p5hgz             0/1     Pending   0          17m
spray-service-78588fd978-4m4db                0/1     Pending   0          17m
sql2ecl-f654698c6-7j2c5                       0/1     Pending   0          17m
thor-eclagent-d5448879-727qt                  0/1     Pending   0          17m
thor-thoragent-6d78484979-t59wj               0/1     Pending   0          17m
```

4. Describe pod
```code
kubectl describe pod hthor-65b4b7b5b6-zfwzc
Name:           hthor-65b4b7b5b6-zfwzc
Namespace:      default
Priority:       0
Node:           virtual-node-aci-linux/
Labels:         accessDali=yes
                accessEsp=no
                app.kubernetes.io/component=hthor
                app.kubernetes.io/instance=hthor
                app.kubernetes.io/managed-by=Helm
                app.kubernetes.io/name=eclagent
                app.kubernetes.io/part-of=HPCC-Platform
                app.kubernetes.io/version=9.0.0
                helm.sh/chart=hpcc-9.0.0
                helmVersion=9.0.0
                instanceOf=hthor
                pod-template-hash=65b4b7b5b6
                run=hthor
Annotations:    checksum/config: a9d7fe1fbb5cf0fb25edc395503f722178e2c60fae259278b2efc2eeaea67121
Status:         Pending
IP:
IPs:            <none>
Controlled By:  ReplicaSet/hthor-65b4b7b5b6
Containers:
  hthor:
    Image:      hpccsystems/platform-core:9.0.0
    Port:       <none>
    Host Port:  <none>
    Command:
      check_executes
    Args:
      -d
      /var/lib/HPCCSystems/debug
      --
      agentexec
      --config=/etc/config/hthor.yaml
      --daliServers=mydali:7070
    Limits:
      cpu:     200m
      memory:  800Mi
    Requests:
      cpu:      200m
      memory:   800Mi
    Readiness:  exec [cat /tmp/hthor.sentinel] delay=0s timeout=1s period=10s #success=1 #failure=3
    Startup:    exec [cat /tmp/hthor.sentinel] delay=0s timeout=1s period=10s #success=1 #failure=30
    Environment:
      SENTINEL:  /tmp/hthor.sentinel
    Mounts:
      /etc/config from hthor-configmap-volume (rw)
      /tmp from hthor-temp-volume (rw)
      /var/lib/HPCCSystems from hthor-hpcctmp-volume (rw)
      /var/lib/HPCCSystems/debug from debug-pv (rw)
      /var/lib/HPCCSystems/hpcc-data from data-pv (rw)
      /var/lib/HPCCSystems/mydropzone from mydropzone-pv (rw)
      /var/lib/HPCCSystems/queries from dll-pv (rw)
      /var/run/secrets/kubernetes.io/serviceaccount from kube-api-access-xcdst (ro)
Conditions:
  Type           Status
  PodScheduled   True
Volumes:
  hthor-temp-volume:
    Type:       EmptyDir (a temporary directory that shares a pod's lifetime)
    Medium:
    SizeLimit:  <unset>
  hthor-hpcctmp-volume:
    Type:       EmptyDir (a temporary directory that shares a pod's lifetime)
    Medium:
    SizeLimit:  <unset>
  hthor-configmap-volume:
    Type:      ConfigMap (a volume populated by a ConfigMap)
    Name:      hthor-configmap
    Optional:  false
  dll-pv:
    Type:              CSI (a Container Storage Interface (CSI) volume source)
    Driver:            file.csi.azure.com
    FSType:
    ReadOnly:          false
    VolumeAttributes:      mountOptions=dir_mode=0777,file_mode=0777,cache=strict,actimeo=30,nosharesock
                           secretName=azure-secret
                           shareName=dllsshare
  data-pv:
    Type:              CSI (a Container Storage Interface (CSI) volume source)
    Driver:            file.csi.azure.com
    FSType:
    ReadOnly:          false
    VolumeAttributes:      mountOptions=dir_mode=0777,file_mode=0777,cache=strict,actimeo=30,nosharesock
                           secretName=azure-secret
                           shareName=datashare
  mydropzone-pv:
    Type:              CSI (a Container Storage Interface (CSI) volume source)
    Driver:            file.csi.azure.com
    FSType:
    ReadOnly:          false
    VolumeAttributes:      mountOptions=dir_mode=0777,file_mode=0777,cache=strict,actimeo=30,nosharesock
                           secretName=azure-secret
                           shareName=dropzone
  debug-pv:
    Type:              CSI (a Container Storage Interface (CSI) volume source)
    Driver:            file.csi.azure.com
    FSType:
    ReadOnly:          false
    VolumeAttributes:      mountOptions=dir_mode=0777,file_mode=0777,cache=strict,actimeo=30,nosharesock
                           secretName=azure-secret
                           shareName=debugshare
  kube-api-access-xcdst:
    Type:                    Projected (a volume that contains injected data from multiple sources)
    TokenExpirationSeconds:  3607
    ConfigMapName:           kube-root-ca.crt
    ConfigMapOptional:       <nil>
    DownwardAPI:             true
QoS Class:                   Guaranteed
Node-Selectors:              kubernetes.io/role=agent
                             type=virtual-kubelet
Tolerations:                 node.kubernetes.io/memory-pressure:NoSchedule op=Exists
                             node.kubernetes.io/not-ready:NoExecute op=Exists for 300s
                             node.kubernetes.io/unreachable:NoExecute op=Exists for 300s
                             virtual-kubelet.io/provider=azure:NoSchedule
Events:
  Type    Reason                 Age   From                                   Message
  ----    ------                 ----  ----                                   -------
  Normal  Scheduled              18m   default-scheduler                      Successfully assigned default/hthor-65b4b7b5b6-zfwzc to virtual-node-aci-linux
  Normal  ProviderCreateSuccess  18m   virtual-node-aci-linux/pod-controller  Create pod in provider successfully
```
```code
kubectl logs hthor-65b4b7b5b6-zfwzc
Error from server (InternalError): Internal error occurred: error getting container logs?): IPAddress cannot be nil for container group default-hthor-65b4b7b5b6-zfwzc
```

  5. Describe node
  ```code
  kubectl describe node virtual-node-aci-linux
Name:               virtual-node-aci-linux
Roles:              agent
Labels:             alpha.service-controller.kubernetes.io/exclude-balancer=true
                    beta.kubernetes.io/os=linux
                    kubernetes.azure.com/managed=false
                    kubernetes.azure.com/role=agent
                    kubernetes.io/hostname=virtual-node-aci-linux
                    kubernetes.io/role=agent
                    node-role.kubernetes.io/agent=
                    node.kubernetes.io/exclude-from-external-load-balancers=true
                    type=virtual-kubelet
Annotations:        node.alpha.kubernetes.io/ttl: 0
                    virtual-kubelet.io/last-applied-node-status:
                      {"capacity":{"cpu":"10k","memory":"4Ti","pods":"5k"},"allocatable":{"cpu":"10k","memory":"4Ti","pods":"5k"},"conditions":[{"type":"Ready",...
                    virtual-kubelet.io/last-applied-object-meta:
                      {"name":"virtual-node-aci-linux","uid":"d279456f-0e24-4126-9f91-21f46d49fb73","creationTimestamp":null,"labels":{"alpha.service-controller...
CreationTimestamp:  Thu, 13 Apr 2023 18:55:03 -0400
Taints:             virtual-kubelet.io/provider=azure:NoSchedule
Unschedulable:      false
Lease:
  HolderIdentity:  virtual-node-aci-linux
  AcquireTime:     <unset>
  RenewTime:       Thu, 13 Apr 2023 19:26:57 -0400
Conditions:
  Type                 Status  LastHeartbeatTime                 LastTransitionTime                Reason                       Message
  ----                 ------  -----------------                 ------------------                ------                       -------
  Ready                True    Thu, 13 Apr 2023 19:26:04 -0400   Thu, 13 Apr 2023 18:55:03 -0400   KubeletReady                 kubelet is ready.
  OutOfDisk            False   Thu, 13 Apr 2023 19:26:04 -0400   Thu, 13 Apr 2023 18:55:03 -0400   KubeletHasSufficientDisk     kubelet has sufficient disk space available
  MemoryPressure       False   Thu, 13 Apr 2023 19:26:04 -0400   Thu, 13 Apr 2023 18:55:03 -0400   KubeletHasSufficientMemory   kubelet has sufficient memory available
  DiskPressure         False   Thu, 13 Apr 2023 19:26:04 -0400   Thu, 13 Apr 2023 18:55:03 -0400   KubeletHasNoDiskPressure     kubelet has no disk pressure
  NetworkUnavailable   False   Thu, 13 Apr 2023 19:26:04 -0400   Thu, 13 Apr 2023 18:55:03 -0400   RouteCreated                 RouteController created a route
Addresses:
  InternalIP:  10.224.0.115
Capacity:
  cpu:     10k
  memory:  4Ti
  pods:    5k
Allocatable:
  cpu:     10k
  memory:  4Ti
  pods:    5k
System Info:
  Machine ID:
  System UUID:
  Boot ID:
  Kernel Version:
  OS Image:
  Operating System:           Linux
  Architecture:               amd64
  Container Runtime Version:
  Kubelet Version:            v1.19.10-vk-azure-aci-1.4.8
  Kube-Proxy Version:
Non-terminated Pods:          (24 in total)
  Namespace                   Name                                           CPU Requests  CPU Limits  Memory Requests  Memory Limits  Age
  ---------                   ----                                           ------------  ----------  ---------------  -------------  ---
  default                     dfs-5866696bbc-67zsj                           0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     dfuserver-55fc6f6dc7-sdcs5                     0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     eclqueries-f94db687b-n7l9h                     0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     eclscheduler-bf98ffcff-8kxqn                   50m (0%)      50m (0%)    200Mi (0%)       200Mi (0%)     21m
  default                     eclservices-55584cbb86-zg65k                   0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     eclwatch-c7cdbb6d5-cxbs2                       0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     esdl-sandbox-74d875bf46-lj947                  0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     hthor-65b4b7b5b6-zfwzc                         200m (0%)     200m (0%)   800Mi (0%)       800Mi (0%)     21m
  default                     mydali-7dcf474db9-z7sn5                        0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     myeclccserver-bbbf6c997-8425k                  200m (0%)     200m (0%)   800Mi (0%)       800Mi (0%)     21m
  default                     roxie-agent-1-848bdb4498-6zgtg                 0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     roxie-agent-1-848bdb4498-cdvdg                 0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     roxie-agent-2-6748f799f7-b824h                 0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     roxie-agent-2-6748f799f7-lmfkl                 0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     roxie-toposerver-555f54544d-fd4fh              50m (0%)      50m (0%)    200M##  0 (0%)           0 (0%)         21m
  default                     thor-eclagent-d5448879-727qt                   0 (0%)        0 (0%)      0 (0%)           0 (0%)         21m
  default                     thor-thoragent-6d78484979-t59wj                100m (0%)     100m (0%)   400Mi (0%)       400Mi (0%)     21m
Allocated resources:
  (Total limits may be over 100 percent, i.e., overcommitted.)
  Resource           Requests     Limits
  --------           --------     ------
  cpu                600m (0%)    600m (0%)
  memory             2400Mi (0%)  2400Mi (0%)
  ephemeral-storage  0 (0%)       0 (0%)
Events:
  Type    Reason          Age   From             Message
  ----    ------          ----  ----             -------
  Normal  RegisteredNode  31m   node-controller  Node virtual-node-aci-linux event: Registered Node virtual-node-aci-linux in Controller
  ```




### mini cluster (hthor)
1. list pods
```console
kubectl get pod
NAME                             READY   STATUS    RESTARTS   AGE
eclwatch-c565cc659-d45mr         0/1     Pending   0          8m47s
hthor-5968578967-59jjg           0/1     Pending   0          8m47s
mydali-7d46c5769c-f4gdv          0/2     Pending   0          8m47s
myeclccserver-7f695ccd7c-gf2cl   0/1     Pending   0          8m47s
spray-service-6977d9d46f-q4jd2   0/1     Pending   0          8m47s
```
2. virtual node
container group only has 400 mi cpu and 1600 MB memory
```console
   ming@RIS-Cfpgz3T8FNL:~/work/HPCC/Cloud/Azure/ACI/HPCC-Platform/helm$ kubectl describe node virtual-node-aci-linux
Name:               virtual-node-aci-linux
Roles:              agent
Labels:             alpha.service-controller.kubernetes.io/exclude-balancer=true
                    beta.kubernetes.io/os=linux
                    kubernetes.azure.com/managed=false
                    kubernetes.azure.com/role=agent
                    kubernetes.io/hostname=virtual-node-aci-linux
                    kubernetes.io/role=agent
                    node-role.kubernetes.io/agent=
                    node.kubernetes.io/exclude-from-external-load-balancers=true
                    type=virtual-kubelet
Annotations:        node.alpha.kubernetes.io/ttl: 0
                    virtual-kubelet.io/last-applied-node-status:
                      {"capacity":{"cpu":"10k","memory":"4Ti","pods":"5k"},"allocatable":{"cpu":"10k","memory":"4Ti","pods":"5k"},"conditions":[{"type":"Ready",...
                    virtual-kubelet.io/last-applied-object-meta:
                      {"name":"virtual-node-aci-linux","uid":"ab9308e3-2aef-486f-b03a-756e9c05e1fc","creationTimestamp":null,"labels":{"alpha.service-controller...
CreationTimestamp:  Mon, 17 Apr 2023 21:54:55 -0400
Taints:             virtual-kubelet.io/provider=azure:NoSchedule
Unschedulable:      false
Lease:
  HolderIdentity:  virtual-node-aci-linux
  AcquireTime:     <unset>
  RenewTime:       Mon, 17 Apr 2023 23:10:36 -0400
Conditions:
  Type                 Status  LastHeartbeatTime                 LastTransitionTime                Reason                       Message
  ----                 ------  -----------------                 ------------------                ------                       -------
  Ready                True    Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   KubeletReady                 kubelet is ready.
  OutOfDisk            False   Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   KubeletHasSufficientDisk     kubelet has sufficient disk space available
  MemoryPressure       False   Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   KubeletHasSufficientMemory   kubelet has sufficient memory available
  DiskPressure         False   Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   KubeletHasNoDiskPressure     kubelet has no disk pressure
  NetworkUnavailable   False   Mon, 17 Apr 2023 23:10:23 -0400   Mon, 17 Apr 2023 22:11:22 -0400   RouteCreated                 RouteController created a route
Addresses:
  InternalIP:  10.224.0.119
Capacity:
  cpu:     10k
  memory:  4Ti
  pods:    5k
Allocatable:
  cpu:     10k
  memory:  4Ti
  pods:    5k
System Info:
  Machine ID:
  System UUID:
  Boot ID:
  Kernel Version:
  OS Image:
  Operating System:           Linux
  Architecture:               amd64
  Container Runtime Version:
  Kubelet Version:            v1.19.10-vk-azure-aci-1.4.8
  Kube-Proxy Version:
Non-terminated Pods:          (5 in total)
  Namespace                   Name                              CPU Requests  CPU Limits  Memory Requests  Memory Limits  Age
  ---------                   ----                              ------------  ----------  ---------------  -------------  ---
  default                     eclwatch-c565cc659-d45mr          0 (0%)        0 (0%)      0 (0%)           0 (0%)         4m3s
  default                     hthor-5968578967-59jjg            200m (0%)     200m (0%)   800Mi (0%)       800Mi (0%)     4m3s
  default                     mydali-7d46c5769c-f4gdv           0 (0%)        0 (0%)      0 (0%)           0 (0%)         4m3s
  default                     myeclccserver-7f695ccd7c-gf2cl    200m (0%)     200m (0%)   800Mi (0%)       800Mi (0%)     4m3s
  default                     spray-service-6977d9d46f-q4jd2    0 (0%)        0 (0%)      0 (0%)           0 (0%)         4m3s
Allocated resources:
  (Total limits may be over 100 percent, i.e., overcommitted.)
  Resource           Requests     Limits
  --------           --------     ------
  cpu                400m (0%)    400m (0%)
  memory             1600Mi (0%)  1600Mi (0%)
  ephemeral-storage  0 (0%)       0 (0%)
Events:              <none>
```



{"code":"MultipleErrorsOccurred","message":"Multiple error occurred: NotFound,NotFound. Please see details.","details":[{"code":"ResourceGroupNotFound","message":"Resource group 'DefaultResourceGroup-EUS' could not be found."},{"code":"ResourceGroupNotFound","message":"Resource group 'DefaultResourceGroup-EUS' could not be found."}]}

### Azure Support ticket startup
TrackingID#2304180040013141
Marjorie Sancho Soto <msanchosoto@microsoft.com>

 kubectl get node virtual-node-aci-linux -o json | grep capacity -A3 -B3

 ### Start up time
 ```console
kubectl get pod
NAME                             READY   STATUS    RESTARTS   AGE
eclservices-576d77b89c-fzfh8     0/1     Pending   0          14m
eclwatch-58c9fcd87d-flrg7        1/1     Running   0          14m
hthor-5d5c8b545d-fdfrz           1/1     Pending   0          14m
mydali-84866fcc9d-plsvm          0/2     Pending   0          14m
myeclccserver-647bc7b46b-jm4vg   1/1     Running   0          14m
```

### Azure Support ticket map ip to hostname
Case 2305310040009164  Your question was successfully submitted to Microsoft Support TrackingID#2305310040009164