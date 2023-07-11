#! /bin/bash
#polic-arn:   arn:aws:iam::446598291512:policy/OIDC-Provider
aws iam create-policy --policy-name OIDC-Provider --policy-document file://OIDC-Provider.json

#aws iam attach-user-policy --user-name hpcc.build --policy-arn arn:aws:iam::446598291512:policy/OIDC-Provider
#An error occurred (AccessDenied) when calling the AttachUserPolicy operation: User: arn:aws:iam::446598291512:user/hpcc.build is not authorized to perform: iam:AttachUserPolicy on resource: user hpcc.build with an explicit deny in a service control policy
