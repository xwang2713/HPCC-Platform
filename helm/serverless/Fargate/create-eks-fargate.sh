#VPC vpc-0cab00867cb81d551
ZONE1=us-east-2a
VPC_PUB_SUBNET1=subnet-063c923bb16ca7413
ZONE2=us-east-2b
VPC_PUB_SUBNET2=subnet-091edce654be335b7
ZONE3=us-east-2c
VPC_PUB_SUBNET3=subnet-037e0ffd150aefb7f

eksctl create cluster \
        --name hpcc-fargate-ming \
        --region us-east-2 \
        --fargate \
        --tags "application=hpccsystems,lifecycle=dev,market=hpccsystems,admin=ming" \
        --tags "owner_email=xiaoming.wang@lexisnexis.com" \
        --tags "support_email=xiaoming.wang@lexisnexis.com" \
        --tags "product=hpccsystems,project=eks fargate,service=eks"

#	--vpc-public-subnets subnet-0163ff5d23876da96 \
#	--vpc-public-subnets subnet-0f650cebc3c2c6cbb \
#	--vpc-public-subnets subnet-092ef6989b21fba78 \
