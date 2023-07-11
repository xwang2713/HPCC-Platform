#! /bin/bash

source ./env

associate_iam_oidc_provider ()
{
eksctl utils associate-iam-oidc-provider \
    --region ${AWS_REGION} \
    --cluster ${EKS_NAME} \
    --approve
}

install_iam_policy () 
{
echo "LBC_VERSION: $LBC_VERSION"
curl -o iam_policy.json https://raw.githubusercontent.com/kubernetes-sigs/aws-load-balancer-controller/${LBC_VERSION}/docs/install/iam_policy.json
aws iam create-policy \
    --policy-name AWSLoadBalancerControllerIAMPolicy \
    --policy-document file://iam_policy.json
rm iam_policy.json
}

install_iamserviceaccount ()
{
eksctl create iamserviceaccount \
  --cluster ${EKS_NAME} \
  --namespace kube-system \
  --name aws-load-balancer-controller \
  --attach-policy-arn arn:aws:iam::${ACCOUNT_ID}:policy/AWSLoadBalancerControllerIAMPolicy \
  --override-existing-serviceaccounts \
  --approve
}

show_lbc_serviceaccount()
{
kubectl get sa aws-load-balancer-controller -n kube-system -o yaml
}

install_targetgroupbinding_crds ()
{
kubectl apply -k "github.com/aws/eks-charts/stable/aws-load-balancer-controller/crds?ref=master"
}

check_lbc_version ()
{
if [ ! -x ${LBC_VERSION} ]
  then
    tput setaf 2; echo "${LBC_VERSION} has been set."
  else
    tput setaf 1;echo "${LBC_VERSION} has NOT been set."
fi
}

install_lbc()
{
echo "Add/update eks/aws-load-balancer-controller chart"
helm repo add eks https://aws.github.io/eks-charts

echo "Get eks/aws-load-balancer-controller chart version"
export LBC_CHART_VERSION=$(helm search repo eks/aws-load-balancer-controller | tail -n 1 | awk '{print $2}')

echo "Get VPC ID"
export VPC_ID=$(aws eks describe-cluster \
                --name ${EKS_NAME} \
                --query "cluster.resourcesVpcConfig.vpcId" \
                --output text)

echo "helm upgrade -i aws-load-balancer-controller \
    eks/aws-load-balancer-controller \
    -n kube-system \
    --set clusterName=eksworkshop-eksctl \
    --set serviceAccount.create=false \
    --set serviceAccount.name=aws-load-balancer-controller \
    --set image.tag=${LBC_VERSION} \
    --set region=${AWS_REGION} \
    --set vpcId=${VPC_ID} \
    --version=${LBC_CHART_VERSION}"

helm upgrade -i aws-load-balancer-controller \
    eks/aws-load-balancer-controller \
    -n kube-system \
    --set clusterName=eksworkshop-eksctl \
    --set serviceAccount.create=false \
    --set serviceAccount.name=aws-load-balancer-controller \
    --set image.tag="${LBC_VERSION}" \
    --set region=${AWS_REGION} \
    --set vpcId=${VPC_ID} \
    --version="${LBC_CHART_VERSION}"

echo "Check aws-load-balancer-controller"
kubectl -n kube-system rollout status deployment aws-load-balancer-controller

}

kubectl -n kube-system rollout status deployment aws-load-balancer-controller  2> /dev/null
if [ $? = 0 ]
then
  echo "aws-load-balancer-controller is already installed"
  exit 0
fi
#associate_iam_oidc_provider 
#install_iam_policy 
#install_iamserviceaccount 
#show_lbc_serviceaccount
#install_targetgroupbinding_crds 
check_lbc_version 
install_lbc
